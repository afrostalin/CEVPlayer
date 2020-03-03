// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "VideoPlayer.h"
#include "Core/Timer.h"
#include "Core/ThreadManager.h"

#include <algorithm>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <ratio>
#include <cstring>

#include "Video/2DVideoQueue.h"
#include "Audio/AudioWrapper.h"

#define NOMINMAX
#include <Windows.h>

namespace CEVPlayer
{

#define _UNUSED(expr) do { (void)(expr); } while (0)

	CVideoPlayer::CVideoPlayer()
		: m_initialized(false)
		, m_looped(false)
		, m_isSkippable(false)
		, m_canBePaused(false)
		, m_reader(nullptr)
		, m_cluster(nullptr)
		, m_audioDecoder(nullptr)
		, m_audioWrapper(nullptr)
		, m_decodeBuffer(nullptr)
		, m_frameBuffer(nullptr)
		, m_blockEntry(nullptr)
		, m_audioTrackIdx(0)
		, m_hasVideo(false)
		, m_hasAudio(false)
		, m_framesDecoded(0)
		, m_state(SVideoPlayerState::Uninitialized)
		, m_rgbaWidth(0)
		, m_rgbaHeight(0)
		, m_rgbaSize(0)
		, m_pSwsContext(nullptr)
		, m_tracks(nullptr)
	{
		m_packetPool = new ObjectPool<CPacket>(1024 * 4);
		reset();
	}

	CVideoPlayer::~CVideoPlayer()
	{
		destroy();
	}

	SVideoPlayerResult CVideoPlayer::load(const char* fileName, bool preloaded, bool looped, int audioTrack, bool isSkippable, bool canBePaused)
	{
		reset();

		m_videoName = fileName;

		string videoFileName;
		videoFileName.Format(VIDEO_FOLDER "%s" VIDEO_FORMAT_EXT, fileName);

		int maj, min, build, rev;
		mkvparser::GetVersion(maj, min, build, rev);
		Log("<CVideoPlayer> libmkv verison: %d.%d.%d.%d\n", maj, min, build, rev);

		Log("<CVideoPlayer> Loading <%s> ...", videoFileName.c_str());

		m_reader = new CVideoFileReader();
		if (m_reader->Open(videoFileName.c_str(), preloaded))
		{
			LogError("<CVideoPlayer> Failed to open video file <%s>", videoFileName.c_str());
			m_reader->Close();
			return SVideoPlayerResult::FileNotExists;
		}

		long long pos = 0;

		long long ret = m_header.Parse(m_reader, pos);
		if (ret < 0)
		{
			LogError("<CVideoPlayer> EBMLHeader::Parse() failed.");
			m_reader->Close();
			return SVideoPlayerResult::FailedParseHeader;
		}
		else
		{
			LogDebug("<CVideoPlayer> ---Header info---");
			LogDebug("<CVideoPlayer> Header ver. : <%d>", m_header.m_version);
			LogDebug("<CVideoPlayer> Header max id lenght : <%d>", m_header.m_maxIdLength);
			LogDebug("<CVideoPlayer> Header max size lenght : <%d>", m_header.m_maxSizeLength);
			LogDebug("<CVideoPlayer> Header doc type : <%s>", m_header.m_docType);
			LogDebug("<CVideoPlayer> Pos : <%d>", pos);
		}

		seg_t* pSegment_ = nullptr;

		ret = seg_t::CreateInstance(m_reader, pos, pSegment_);
		if (ret)
		{
			LogError("<CVideoPlayer> Segment::CreateInstance() failed.");
			m_reader->Close();
			return SVideoPlayerResult::FailedCreateInstance;
		}

		m_segment.reset(pSegment_);

		ret = m_segment->Load();
		if (ret < 0)
		{
			LogError("<CVideoPlayer> Segment::Load() failed, error <%d>", ret);
			m_reader->Close();
			return SVideoPlayerResult::FailedLoadSegment;
		}

		const mkvparser::SegmentInfo* const pSegmentInfo = m_segment->GetInfo();
		if (pSegmentInfo == nullptr)
		{
			LogError("<CVideoPlayer> Segment::GetInfo() failed.");
			m_reader->Close();
			return SVideoPlayerResult::FailedGetSegmentInfo;
		}

		m_info.duration = (float)(pSegmentInfo->GetDuration() / NS_PER_S);

		LogDebug("<CVideoPlayer> ---Segment info---");
		LogDebug("<CVideoPlayer> TimeCodeScale : %lld", pSegmentInfo->GetTimeCodeScale());
		LogDebug("<CVideoPlayer> Duration(secs) : %7.3lf", m_info.duration);
		LogDebug("<CVideoPlayer> Position(Segment) : %lld", pSegmentInfo->m_start);
		LogDebug("<CVideoPlayer> Size(Segment) : %lld", pSegmentInfo->m_size);

		m_tracks = m_segment->GetTracks();

		unsigned long track_num = 0;
		const unsigned long num_tracks = m_tracks->GetTracksCount();
		int currentAudioTrack = 0;

		m_hasVideo = false;
		m_hasAudio = false;

		LogDebug("<CVideoPlayer> ---Track info---");

		while (track_num != num_tracks)
		{
			const mkvparser::Track* const pTrack = m_tracks->GetTrackByIndex(track_num++);

			if (pTrack == NULL)
				continue;

			const long trackType = pTrack->GetType();
			const long trackNumber = pTrack->GetNumber();
			const unsigned long long trackUid = pTrack->GetUid();

			if (trackType == mkvparser::Track::kVideo)
			{
				const mkvparser::VideoTrack* const pVideoTrack = static_cast<const mkvparser::VideoTrack*>(pTrack);

				m_info.width = (int)pVideoTrack->GetWidth();
				m_info.height = (int)pVideoTrack->GetHeight();
				m_info.frameRate = 1.0f / (float)(pVideoTrack->GetDefaultDuration() / NS_PER_S);
				m_info.decodeThreadsCount = 1;

				LogDebug("<CVideoPlayer> Track Type : %ld", trackType);
				LogDebug("<CVideoPlayer> Track Number : %ld", trackNumber);
				LogDebug("<CVideoPlayer> Track Uid : %lld", trackUid);
				LogDebug("<CVideoPlayer> Video Width : %lld", m_info.width);
				LogDebug("<CVideoPlayer> Video Height : %lld", m_info.height);
				LogDebug("<CVideoPlayer> Video Rate : %f", m_info.frameRate);

				// configure codec
				const char* codecId = pVideoTrack->GetCodecId();

				if (!strcmp(codecId, "V_VP8"))
				{
					m_decoderData.iface = &vpx_codec_vp8_dx_algo;
				}
				else if (!strcmp(codecId, "V_VP9"))
				{
					m_decoderData.iface = &vpx_codec_vp9_dx_algo;
				}
				else
				{
					LogError("<CVideoPlayer> Unsupported video codec: %s", codecId);
					m_reader->Close();
					return SVideoPlayerResult::UnsupportedVideoCodec;
				}

				vpx_codec_dec_cfg_t vpx_config = { 0 };
				vpx_config.w = m_info.width;
				vpx_config.h = m_info.height;
				vpx_config.threads = m_info.decodeThreadsCount;

				// initialize decoder
				if (vpx_codec_dec_init(&m_decoderData.codec, m_decoderData.iface, &vpx_config, m_decoderData.flags))
				{
					LogError("<CVideoPlayer> Failed to initialize decoder (%s)", vpx_codec_error_detail(&m_decoderData.codec));
					m_reader->Close();
					return SVideoPlayerResult::FailedInitializeVideoDecoder;
				}

				// alloc framebuffer
				m_frameBuffer = new CVideoFrameBuffer(this, m_info.width, m_info.height, 4);

				// video width, height and frame size
				m_rgbaWidth = (m_info.width >> 2) << 2;
				m_rgbaHeight = (m_info.height >> 2) << 2;
				m_rgbaSize = m_rgbaWidth * m_rgbaHeight * 4;

				if (m_pSwsContext == nullptr)
				{
					m_pSwsContext = sws_getContext(m_rgbaWidth, m_rgbaHeight, AV_PIX_FMT_YUV420P,
						m_rgbaWidth, m_rgbaHeight, AV_PIX_FMT_RGBA,
						SWS_BILINEAR, nullptr, nullptr, nullptr);

					if (m_pSwsContext == nullptr)
					{
						LogError("<CRenderWrapper> Can't create sws context!");
						return SVideoPlayerResult::FailedInitializeVideoDecoder;
					}
				}
				else
				{
					LogError("<CRenderWrapper> Can't create sws context - previous not deleted!");
					return SVideoPlayerResult::FailedInitializeVideoDecoder;
				}

				m_decoderData.initialized = true;
				m_hasVideo = true;
				m_looped = looped;
				m_isSkippable = isSkippable;
				m_canBePaused = canBePaused;

				m_info.videoFileName = fileName;

				Log("<CVideoPlayer> Video <%s> opened", videoFileName.c_str());
			}

			if (trackType == mkvparser::Track::kAudio)
			{
				currentAudioTrack++;

				if (audioTrack == currentAudioTrack - 1)
				{
					m_audioTrackIdx = trackNumber;

					const mkvparser::AudioTrack* const pAudioTrack = static_cast<const mkvparser::AudioTrack*>(pTrack);

					// Don't leak audio decoders
					SAFE_DELETE_11(m_audioDecoder);

					m_audioDecoder = new CAudioDecoder(this, 4 * 1024);
					m_audioDecoder->init();

					// parse Vorbis headers
					size_t size;
					unsigned char* data = (unsigned char*)pAudioTrack->GetCodecPrivate(size);

					if (!m_audioDecoder->initHeader(data, size))
					{
						LogError("<CVideoPlayer> Failed to decode audio header");
						SAFE_DELETE_11(m_audioDecoder);
					}
					else
					{
						if (!m_audioDecoder->postInit())
						{
							LogError("<CVideoPlayer> Failed to post-init audio decoder");
							SAFE_DELETE_11(m_audioDecoder);
						}
						else
						{
							m_info.audioChannels = m_audioDecoder->channels();
							m_info.audioFrequency = m_audioDecoder->rate();
							m_info.audioSamples = (int)((m_info.audioChannels * m_info.audioFrequency) * m_info.duration);

							m_info.hasAudio = true;

							m_hasAudio = true;

							m_audioWrapper = new CAudioWrapper(this);

							LogDebug("<CVideoPlayer> Audio track with id <%d> found!", audioTrack);
						}
					}
				}
			}
		}

		m_decodeBuffer = new Buffer<unsigned char>(2 * 1024 * 1024);

		m_state = SVideoPlayerState::Initialized;
		m_initialized = true;

		return SVideoPlayerResult::Success;
	}

	void CVideoPlayer::destroy()
	{
		m_videoQueue.destroy();
		m_audioQueue.destroy();

		m_Listeners.clear();

		SAFE_DELETE_11(m_audioDecoder);
		SAFE_DELETE_11(m_audioWrapper);
		SAFE_DELETE_11(m_decodeBuffer);
		SAFE_DELETE_11(m_reader);

		if (m_decoderData.initialized)
		{
			vpx_codec_destroy(&m_decoderData.codec);
		}

		SAFE_DELETE_11(m_frameBuffer);

		SAFE_DELETE_11(m_packetPool);

		sws_freeContext(m_pSwsContext);
		m_pSwsContext = nullptr;
	}

	void CVideoPlayer::reset()
	{
		memset(&m_decoderData, 0, sizeof(m_decoderData));

		m_hasVideo = false;
		m_hasAudio = false;

		m_videoName.clear();
	}

	bool CVideoPlayer::update()
	{
		bool callOnFinishEvent = false;

		m_updateMutex.lock();
		{
			if (m_state == SVideoPlayerState::Playing)
			{
				if (playTime() >= duration())
				{
					if (!m_looped)
					{
						m_state = SVideoPlayerState::Finished;
					}
					else
					{
						m_state = SVideoPlayerState::Restarting;
						restart();
					}

					callOnFinishEvent = true;
				}
			}
		}
		m_updateMutex.unlock();

		if (callOnFinishEvent)
		{
			Log("<CVideoPlayer> Video <%s> finished, is loop <%s>", m_reader->GetFileName().c_str(), m_looped ? "true" : "false");
		}

		return true;
	}

	void  CVideoPlayer::updateYUVData(double time)
	{
		if (m_decoderData.img == nullptr)
			return;

		CVideoFrame* curYUV = m_frameBuffer->lockWrite(time);
		if (curYUV != nullptr)
		{
			int linesize[1];
			linesize[0] = curYUV->displayWidth() * 4;

			uint8_t* outData[1];
			outData[0] = curYUV->rgba();

			sws_scale(m_pSwsContext, m_decoderData.img->planes, m_decoderData.img->stride, 0, curYUV->displayHeight(), outData, linesize);
		}

		m_frameBuffer->unlockWrite();
	}

	double CVideoPlayer::playTime()
	{
		return m_timer.elapsedSeconds();
	}

	void  CVideoPlayer::play()
	{
		if (!m_initialized)
		{
			LogError("<CVideoPlayer> Can't play video, because it's not init!");
			return;
		}

		if (m_state == SVideoPlayerState::Paused)
		{
			Log("<CVideoPlayer> Resuming video <%s>...", m_reader->GetFileName().c_str());
			m_state = SVideoPlayerState::Playing;
			m_timer.resume();

			Log("<CVideoPlayer> Video <%s> resumed", m_reader->GetFileName().c_str());

			ExecuteEvent(EVideoPlayerEvents::OnResume);
		}
		else
		{
			Log("<CVideoPlayer> Buffering video <%s>...", m_reader->GetFileName().c_str());

			m_cluster = m_segment->GetFirst();
			m_state = SVideoPlayerState::Buffering;

			mEnv->pThreadManager->PushPlayer(this);
		}
	}

	void  CVideoPlayer::pause()
	{
		if (!m_initialized)
		{
			LogError("<CVideoPlayer> Can't pause video, because it's not init!");
			return;
		}

		Log("<CVideoPlayer> Pause video <%s>...", m_reader->GetFileName().c_str());

		ExecuteEvent(EVideoPlayerEvents::OnPause);

		m_state = SVideoPlayerState::Paused;
		m_timer.pause();

		Log("<CVideoPlayer> Video <%s> paused", m_reader->GetFileName().c_str());
	}

	void  CVideoPlayer::stop()
	{
		if (!m_initialized)
		{
			LogError("<CVideoPlayer> Can't stop video, because it's not init!");
			return;
		}

		Log("<CVideoPlayer> Stop video <%s>...", m_reader->GetFileName().c_str());

		std::lock_guard<std::mutex> lock(m_updateMutex);
		_UNUSED(lock);

		if (m_state != SVideoPlayerState::Stopped)
		{
			m_state = SVideoPlayerState::Stopped;

			if (m_audioDecoder != nullptr)
			{
				m_audioDecoder->resetDecode();
			}

			mEnv->pThreadManager->RemovePlayer(this);

			m_framesDecoded = 0;

			m_videoQueue.destroy();
			m_audioQueue.destroy();

			m_frameBuffer->reset();

			m_blockEntry = nullptr;
			m_cluster = nullptr;

			m_timer.stop();

			Log("<CVideoPlayer> Video <%s> stopped", m_reader->GetFileName().c_str());
		}

		ExecuteEvent(EVideoPlayerEvents::OnStop);
	}

	void CVideoPlayer::restart()
	{
		Log("<CVideoPlayer> Restart video <%s>...", m_reader->GetFileName().c_str());

		// Reset all
		if (m_audioDecoder)
		{
			m_audioDecoder->resetDecode();
		}

		m_framesDecoded = 0;

		m_videoQueue.destroy();
		m_audioQueue.destroy();

		if (m_frameBuffer)
		{
			m_frameBuffer->reset();
		}

		m_blockEntry = nullptr;
		m_cluster = nullptr;

		m_timer.stop();

		// Restart it
		m_cluster = m_segment->GetFirst();
		m_state = SVideoPlayerState::Buffering;
	}

	const char* CVideoPlayer::getStateStr()
	{
		const char* str = nullptr;

		if (m_state == SVideoPlayerState::Uninitialized)
			str = "Uninitialized";
		else if (m_state == SVideoPlayerState::Initialized)
			str = "Initialized";
		else if (m_state == SVideoPlayerState::Buffering)
			str = "Buffering";
		else if (m_state == SVideoPlayerState::Playing)
			str = "Playing";
		else if (m_state == SVideoPlayerState::Paused)
			str = "Paused";
		else if (m_state == SVideoPlayerState::Stopped)
			str = "Stopped";
		else if (m_state == SVideoPlayerState::Finished)
			str = "Finished";
		else if (m_state == SVideoPlayerState::Restarting)
			str = "Restarting";

		return str;
	}

	void CVideoPlayer::addAudioData(float* values, size_t count)
	{
		if (m_audioWrapper != nullptr)
		{
			m_audioWrapper->OnAudiDataDecoded(values, count);
		}
	}

	void CVideoPlayer::registerEventListener(IVideoPlayerEventListener* listener)
	{
		if (listener == nullptr)
			return;

		for (const auto& it : m_Listeners)
		{
			if (it == listener)
			{
				LogError("Can't register video player listener - alredy registered");
				return;
			}
		}

		m_Listeners.push_back(listener);
	}

	void CVideoPlayer::unregisterEventListener(IVideoPlayerEventListener* listener)
	{
		for (auto it = m_Listeners.begin(); it != m_Listeners.end(); ++it)
		{
			if ((*it) == listener)
			{
				m_Listeners.erase(it);
				break;
			}
		}
	}

	void  CVideoPlayer::decode()
	{
		m_decodeMutex.lock();

		if (m_hasVideo)
		{
			CPacket* videoPacket = getPacket(CPacket::Type::Video);

			while (videoPacket && !m_frameBuffer->isFull())
			{
				decodePacket(videoPacket);
				m_videoQueue.pop();
				videoPacket = getPacket(CPacket::Type::Video);
			}

			m_frameBuffer->update(playTime(), 1.0 / m_info.frameRate);

			if (m_state == SVideoPlayerState::Buffering)
			{
				Log("<CVideoPlayer> Video <%s> buffered", m_reader->GetFileName().c_str());

				ExecuteEvent(EVideoPlayerEvents::OnPlay);

				m_state = SVideoPlayerState::Playing;
				m_timer.start();

				Log("<CVideoPlayer> Playing video <%s>...", m_reader->GetFileName().c_str());
			}
		}

		if (m_hasAudio)
		{
			// how much audio seconds ahead should be decoded
			const double audioPreloadAdvance = 0.2;
			const double preloadTime = playTime() + audioPreloadAdvance;

			CPacket* audioPacket = getPacket(CPacket::Type::Audio);
			while (audioPacket && audioPacket->time() <= preloadTime)
			{
				decodePacket(audioPacket);
				m_audioQueue.pop();
				audioPacket = getPacket(CPacket::Type::Audio);
			}
		}

		m_decodeMutex.unlock();
	}

	CPacket* CVideoPlayer::demuxPacket()
	{
		if (m_blockEntry && m_blockEntry->EOS())
		{
			return nullptr;
		}
		else if (m_blockEntry == nullptr && m_cluster && !m_cluster->EOS())
		{
			long status = m_cluster->GetFirst(m_blockEntry);
			if (status < 0)
			{
				LogError("<CVideoPlayer> Error parsing first block of cluster");
				return nullptr;
			}
		}

		CPacket* ret = nullptr;

		while (m_cluster && m_blockEntry)
		{
			const mkvparser::Block* const pBlock = m_blockEntry->GetBlock();

			if (pBlock == nullptr)
			{
				LogError("<CVideoPlayer> Can't get block");
				return nullptr;
			}

			const long long trackNum = pBlock->GetTrackNumber();
			const unsigned long tn = static_cast<unsigned long>(trackNum);
			const mkvparser::Track* const pTrack = m_tracks->GetTrackByNumber(tn);
			const long long time_ns = pBlock->GetTime(m_cluster);
			const double time_sec = (double(time_ns) / NS_PER_S);

			if (pTrack == nullptr)
			{
				LogError("<CVideoPlayer> Unknown block track type");
				return nullptr;
			}

			switch (pTrack->GetType())
			{
			case mkvparser::Track::kVideo:
			{
				ret = m_packetPool->GetNextWithoutInitializing();
				ret = new(ret) CPacket(pBlock, CPacket::Type::Video, time_sec);

				m_videoQueue.push(ret);
				break;
			}

			case mkvparser::Track::kAudio:
			{
				if (trackNum == m_audioTrackIdx)
				{
					ret = m_packetPool->GetNextWithoutInitializing();
					ret = new(ret) CPacket(pBlock, CPacket::Type::Audio, time_sec);

					m_audioQueue.push(ret);
				}
				break;
			}
			}

			long status = m_cluster->GetNext(m_blockEntry, m_blockEntry);
			if (status < 0)
			{
				LogError("<CVideoPlayer> Error parsing next block of cluster");
				return ret;
			}

			if (m_blockEntry == nullptr || m_blockEntry->EOS())
			{
				m_cluster = m_segment->GetNext(m_cluster);
			}

			if (ret)
				return ret;
		}

		return ret;
	}

	void  CVideoPlayer::decodePacket(CPacket* p)
	{
		const mkvparser::Block* pBlock = p->block();
		const int frameCount = pBlock->GetFrameCount();
		const long long discard_padding = pBlock->GetDiscardPadding();

		for (int i = 0; i < frameCount; ++i)
		{
			const mkvparser::Block::Frame& theFrame = pBlock->GetFrame(i);
			const long size = theFrame.len;
			unsigned char* data = m_decodeBuffer->get(size);
			theFrame.Read(m_reader, data);

			switch (p->type())
			{
			case CPacket::Type::Video:
			{
				vpx_codec_stream_info_t si;
				memset(&si, 0, sizeof(si));
				si.sz = sizeof(si);
				vpx_codec_peek_stream_info(m_decoderData.iface, data, size, &si);

				vpx_codec_err_t e = vpx_codec_decode(&m_decoderData.codec, data, size, NULL, 0);
				if (e)
				{
					LogError("<CVideoPlayer> Failed to decode frame (%s) !!!", vpx_codec_err_to_string(e));
					return;
				}

				vpx_codec_iter_t  iter = NULL;
				vpx_image* img = nullptr;

				while ((img = vpx_codec_get_frame(&m_decoderData.codec, &iter)))
				{
					if (img->fmt != VPX_IMG_FMT_I420)
					{
						LogError("<CVideoPlayer> Unsupported image format: %d", img->fmt);
						break;
					}

					m_decoderData.img = img;

					updateYUVData(p->time());
					m_framesDecoded++;
				}

				break;
			}
			case CPacket::Type::Audio:
			{
				if (!m_audioDecoder)
					break;

				int32_t total_frames = 0;
				int aerr = m_audioDecoder->decode(data, size, 0, 0, 0, discard_padding, &total_frames);
				if (aerr)
				{
					LogError("<CVideoPlayer> Failed to decode audio packet (%d)", aerr);
					return;
				}
			}

			default:
				break;
			}
		}

		m_packetPool->DeleteWithoutDestroying(p);
	}

	void CVideoPlayer::ExecuteEvent(EVideoPlayerEvents event)
	{
		if (gEnv->pSystem != nullptr && gEnv->pSystem->IsQuitting())
			return;

		if (m_audioWrapper != nullptr)
		{
			m_audioWrapper->OnVideoPlayerEvent(event); // TODO - Make it listener
		}

		for (const auto& it : m_Listeners)
		{
			it->OnVideoPlayerEvent(m_videoName.c_str(), event);
		}
	}

	CPacket* CVideoPlayer::getPacket(CPacket::Type type)
	{
		CPacket* p = nullptr;

		do
		{
			switch (type)
			{
			case CPacket::Type::Audio:
				p = m_audioQueue.first();
				break;

			case CPacket::Type::Video:
				p = m_videoQueue.first();
				break;
			}

			if (p == nullptr)
			{
				CPacket* demux = demuxPacket();
				if (demux == nullptr)
					return nullptr;
			}

		} while (p == nullptr);

		return p;
	}
}