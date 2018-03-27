// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <math.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <chrono>

#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>

#include <mkvparser/mkvparser.hpp>

#include "Audio/AudioDecoder.h"

#include "Render/FrameBuffer.h"

#include "Core/Timer.h"

#include "Utils/Buffer.h"
#include "Utils/PacketQueue.h"
#include "Utils/ObjectsPool.h"

#include "VideoFileReader.h"
#include "Packet.h"

#include <VideoPlugin/IVideoPluginListeners.h>

extern "C"
{
	#pragma warning( disable : 4244 )
	#include <libswscale/swscale.h>
}

enum class SVideoPlayerState
{
	Uninitialized,
	Initialized,
	Buffering,
	Playing,
	Paused,
	Stopped,
	Finished,
	Restarting
};

enum class SVideoPlayerResult
{
	FileNotExists,
	FailedParseHeader,
	FailedCreateInstance,
	FailedLoadSegment,
	FailedGetSegmentInfo,
	UnsupportedVideoCodec,
	FailedInitializeVideoDecoder,
	Success
};

struct SVideoInfo
{
	int width = 0;
	int height = 0;
	float duration = 0.0f;
	float frameRate = 0.0f;
	int hasAudio = 0;
	int audioChannels = 0;
	int audioFrequency = 0;
	int audioSamples = 0;
	int decodeThreadsCount = 0;
	std::string videoFileName = "";
};

struct SVpxData
{
	vpx_codec_ctx_t     codec;
	vpx_codec_iter_t    iter;
	vpx_image_t         *img;
	vpx_codec_iface_t   *iface;
	vpx_codec_flags_t   flags;
	bool                initialized;
};

static const double NS_PER_S = 1e9;
typedef mkvparser::Segment seg_t;

class CAudioWrapper;

class CVideoPlayer
{
public:
	CVideoPlayer();
	~CVideoPlayer();
public:
	SVideoPlayerResult             load(const char *fileName, bool preloaded = false, bool looped = false, int audioTrack = 0, bool isSkippable = false, bool canBePaused = false);
	bool                           update();

	SVideoInfo                     info() const { return m_info; }
	CVideoFrameBuffer*             frameBuffer() { return m_frameBuffer; }

	double                         playTime();
	float                          duration() { return m_info.duration; }

	void                           play();
	void                           pause();
	void                           stop();
	void                           restart(); // Used for restart video when it looped

	bool                           isStopped() { return m_state == SVideoPlayerState::Stopped; }
	bool                           isPaused() { return m_state == SVideoPlayerState::Paused; }
	bool                           isPlaying() { return m_state == SVideoPlayerState::Playing; }
	bool                           isBuffering() { return m_state == SVideoPlayerState::Buffering; }
	bool                           isFinished() { return m_state == SVideoPlayerState::Finished; }
	bool                           isRestarting() { return m_state == SVideoPlayerState::Restarting; }

	bool                           isSkippable() const { return m_isSkippable; }
	bool                           isCanBePaused() const { return m_canBePaused; }

	const char*                    getStateStr();
	void                           addAudioData(float *values, size_t count);
public:
	void                           registerEventListener(IVideoPlayerEventListener* listener);
	void                           unregisterEventListener(IVideoPlayerEventListener* listener);
public:
	void                           decode();
private:
	void                           reset();
	void                           destroy();
	void                           updateYUVData(double time);
	CPacket*                       demuxPacket();
	CPacket*                       getPacket(CPacket::Type type);
	void                           decodePacket(CPacket *p);
	void                           ExecuteEvent(EVideoPlayerEvents event);
private:
	bool                           m_initialized;
	bool                           m_looped;
	bool                           m_isSkippable;
	bool                           m_canBePaused;

	SVpxData                       m_decoderData;
	CVideoFileReader*              m_reader;
	mkvparser::EBMLHeader          m_header;
	std::unique_ptr<seg_t>         m_segment;
	const mkvparser::Cluster*      m_cluster;
	const mkvparser::Tracks*       m_tracks;
	CAudioDecoder*                 m_audioDecoder;
	CAudioWrapper*                 m_audioWrapper;
	Buffer<unsigned char>*         m_decodeBuffer;
	CVideoFrameBuffer*             m_frameBuffer;
	const mkvparser::BlockEntry*   m_blockEntry;
	long                           m_audioTrackIdx;
	bool                           m_hasVideo;
	bool                           m_hasAudio;
	std::atomic<size_t>            m_framesDecoded;
	CVideoPlayerTimer              m_timer;

	std::mutex                     m_updateMutex;
	std::mutex                     m_decodeMutex;;

	ThreadSafeQueue<CPacket*>      m_videoQueue;
	ThreadSafeQueue<CPacket*>      m_audioQueue;

	ObjectPool<CPacket>*           m_packetPool;

	unsigned int                   m_rgbaWidth;
	unsigned int                   m_rgbaHeight;
	unsigned int                   m_rgbaSize;

	std::atomic<SVideoPlayerState> m_state;
	SVideoInfo                     m_info;

	std::vector<IVideoPlayerEventListener*> m_Listeners;
	SwsContext*                    m_pSwsContext;
};