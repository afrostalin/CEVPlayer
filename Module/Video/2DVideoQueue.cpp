// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "2DVideoQueue.h"
#include "Video/VideoPlayer.h"

#include "Render/Frame.h"
#include "Render/RenderWrapper.h"

#include "Video/VideoPlayer.h"

#include "Input/InputDispatcher.h"

C2DVideoQueue::C2DVideoQueue()
	: m_p2DVideoPlayer(nullptr)
	, m_pRenderWrapper(nullptr)
	, m_p2DVideoBuffer(nullptr)
	, m_lastFrameTime(0.0)
{
	if (gEnv->pGameFramework != nullptr)
	{
		gEnv->pGameFramework->RegisterListener(this, "C2DVideoQueue_GameFrameworkListener", FRAMEWORKLISTENERPRIORITY_DEFAULT);
	}

	if (gEnv->pSystem != nullptr)
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "C2DVideoQueue_Listener");
	}

	m_pRenderWrapper = new CRenderWrapper();

	if (mEnv->pInputDispatcher != nullptr)
	{
		mEnv->pInputDispatcher->RegisterAction("video_plugin", "pause_ingame_video", [this](int activationMode, float value){OnActionPause2DVideo((EInputState)activationMode, value);});
		mEnv->pInputDispatcher->RegisterAction("video_plugin", "skip_ingame_video", [this](int activationMode, float value){OnActionSkip2DVideo((EInputState)activationMode, value);});

		mEnv->pInputDispatcher->BindAction("video_plugin", "pause_ingame_video", "Shift", eAID_KeyboardMouse, EKeyId::eKI_Space);
		mEnv->pInputDispatcher->BindAction("video_plugin", "skip_ingame_video", "Enter", eAID_KeyboardMouse, EKeyId::eKI_Enter);
		mEnv->pInputDispatcher->BindAction("video_plugin", "skip_ingame_video", "Escape", eAID_KeyboardMouse, EKeyId::eKI_Escape);
		mEnv->pInputDispatcher->BindAction("video_plugin", "skip_ingame_video", "Space", eAID_KeyboardMouse, EKeyId::eKI_Escape);
	}
	else
	{
		LogError("<C2DVideoQueue> Can't get input dispatcher!");
	}
}

C2DVideoQueue::~C2DVideoQueue()
{
	SAFE_DELETE_11(m_pRenderWrapper);
	SAFE_DELETE_11(m_p2DVideoPlayer);
	SAFE_DELETE_11(m_p2DVideoBuffer);
}

void C2DVideoQueue::Release()
{
	if (gEnv->pGameFramework != nullptr)
	{
		gEnv->pGameFramework->UnregisterListener(this);
	}

	if (gEnv->pSystem != nullptr)
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
	}

	Stop2DVideo();

	if (m_pRenderWrapper != nullptr)
	{
		m_pRenderWrapper->Release2DVideoTextures();
	}
}

void C2DVideoQueue::OnPostUpdate(float fDeltaTime)
{
	if (m_p2DVideoPlayer != nullptr && m_p2DVideoPlayer->isFinished())
	{
		Stop2DVideo();
	}

	Draw2DVideo(fDeltaTime);

	if (m_p2DVideoPlayer != nullptr)
	{
		m_p2DVideoPlayer->update();
	}
}

void C2DVideoQueue::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
	case ESYSTEM_EVENT_FULL_SHUTDOWN:
	{
		LogDebug("Executed full shutdown - stop 2D video");
		Stop2DVideo();
		break;
	}
	case ESYSTEM_EVENT_FAST_SHUTDOWN:
	{
		LogDebug("Executed fast shutdown - stop 2D video");
		Stop2DVideo();
		break;
	}
	default:
		break;
	}
}

void C2DVideoQueue::Play2DVideo(const char * videoName, bool preload, bool looped, int audioTrack, bool isSkippable, bool canBePaused, bool isLevelLoading)
{
	if (gEnv->pSystem->IsQuitting())
		return;

	if (m_p2DVideoPlayer != nullptr || m_p2DVideoBuffer != nullptr)
	{
		LogError("Can't play 2D video because some video alredy playing");
	}
	else
	{
		if (isLevelLoading)
		{
			m_pRenderWrapper->Create2DVideoTextures();
		}

		m_p2DVideoPlayer = new CVideoPlayer();

		SVideoPlayerResult result = m_p2DVideoPlayer->load(videoName, preload, looped, audioTrack, isSkippable, canBePaused);
		switch (result)
		{
		case SVideoPlayerResult::FileNotExists:
			LogError("Can't play 2D video because video <%s> not exists", videoName);
			break;
		case SVideoPlayerResult::FailedParseHeader:
			LogError("Can't play 2D video because can't parse header");
			break;
		case SVideoPlayerResult::FailedCreateInstance:
			LogError("Can't play 2D video because failed create instance");
			break;
		case SVideoPlayerResult::FailedLoadSegment:
			LogError("Can't play 2D video because failed load segment");
			break;
		case SVideoPlayerResult::FailedGetSegmentInfo:
			LogError("Can't play 2D video because failed get segment info");
			break;
		case SVideoPlayerResult::UnsupportedVideoCodec:
			LogError("Can't play 2D video - unsupported video codec");
			break;
		case SVideoPlayerResult::FailedInitializeVideoDecoder:
			LogError("Can't play 2D video - failed init video decoder");
			break;
		case SVideoPlayerResult::Success:
		{
			SVideoInfo info = m_p2DVideoPlayer->info();

			m_p2DVideoBuffer = new CVideoFrame(info.width, info.height);

			m_lastFrameTime = 0.0;

			m_p2DVideoPlayer->play();

			m_videoFileName = videoName;

			return;
		}
		default:
			break;
		}

		SAFE_DELETE_11(m_p2DVideoPlayer);

		if (isLevelLoading)
		{
			m_pRenderWrapper->Release2DVideoTextures();
		}
	}
}

void C2DVideoQueue::Pause2DVideo()
{
	if (m_p2DVideoPlayer != nullptr && !m_p2DVideoPlayer->isPaused())
	{
		CVideoFrame* pFrame = nullptr;

		if ((pFrame = m_p2DVideoPlayer->frameBuffer()->lockRead()) != nullptr)
		{	
			pFrame->copy(m_p2DVideoBuffer); // Copy last frame before pause video

			m_p2DVideoPlayer->frameBuffer()->unlockRead();
		}

		m_p2DVideoPlayer->pause();
	}
	else
	{
		LogError("Can't pause 2D video because no video in queue or video alredy paused");
	}
}

void C2DVideoQueue::Resume2DVideo()
{
	if (m_p2DVideoPlayer != nullptr && !m_p2DVideoPlayer->isPlaying())
	{
		m_p2DVideoPlayer->play();
	}
	else
	{
		LogError("Can't resume 2D video because no video in queue or video aldery resumed");
	}
}

void C2DVideoQueue::Stop2DVideo()
{
	if (m_p2DVideoPlayer != nullptr && !m_p2DVideoPlayer->isStopped())
	{
		Log("<C2DVideoQueue> Video stopping. Last frame time <%0.3f>, supposed frame time <%0.3f>",(float)m_lastFrameTime, m_p2DVideoPlayer->info().duration);

		m_p2DVideoPlayer->stop();

		SAFE_DELETE_11(m_p2DVideoPlayer);
		SAFE_DELETE_11(m_p2DVideoBuffer);

		m_pRenderWrapper->Release2DVideoTextures();

		ExecuteVideoEvent(EVideoPlayerEvents::OnStop);
	}
	else
	{
		if (gEnv->pSystem != nullptr && !gEnv->pSystem->IsQuitting())
		{
			LogError("Can't stop 2D video because no video in queue or video alredy stopped");
		}
	}
}

bool C2DVideoQueue::Is2DVideoPlayingNow()
{
	if (m_p2DVideoPlayer != nullptr && m_p2DVideoPlayer->isPlaying())
	{
		return true;
	}

	return false;
}

void C2DVideoQueue::OnActionPause2DVideo(EInputState activationMode, float value)
{
	if (activationMode == eIS_Pressed && m_p2DVideoPlayer != nullptr && m_p2DVideoPlayer->isCanBePaused())
	{
		if (m_p2DVideoPlayer->isPlaying())
		{
			Pause2DVideo();
		}
		else if (m_p2DVideoPlayer->isPaused())
		{
			Resume2DVideo();
		}
	}
}

void C2DVideoQueue::OnActionSkip2DVideo(EInputState activationMode, float value)
{
	if (activationMode == eIS_Pressed)
	{
		ExecuteVideoEvent(EVideoPlayerEvents::OnSkip);
	}

	if (activationMode == eIS_Pressed && m_p2DVideoPlayer != nullptr && m_p2DVideoPlayer->isSkippable())
	{
		if (m_p2DVideoPlayer->isPlaying())
		{
			Stop2DVideo();
		}
	}
}

void C2DVideoQueue::Register2DVideoPlayerListener(IVideoPlayerEventListener * listener)
{
	if (stl::find(m_Listeners, listener))
		return;

	m_Listeners.push_back(listener);
}

void C2DVideoQueue::Unregister2DVideoPlayerListener(IVideoPlayerEventListener * listener)
{
	if (m_p2DVideoPlayer != nullptr)
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
}

void C2DVideoQueue::Draw2DVideo(float fDeltaTime)
{
	if (m_p2DVideoPlayer == nullptr 
		|| m_p2DVideoPlayer->frameBuffer() == nullptr 
		|| m_p2DVideoPlayer->isStopped()
		|| m_p2DVideoBuffer == nullptr 
		|| m_pRenderWrapper == nullptr)
	{
		return;
	}

	double frameDelay = 0.0;
	constexpr double normalFrameDelay = (double)(1.0 / 60.0); // 60 FPS <-- TODO 

	CVideoFrame* pFrame = m_p2DVideoPlayer->frameBuffer()->lockRead();

	if (pFrame != nullptr)
	{
		if (m_p2DVideoPlayer->isPaused())
		{
			m_pRenderWrapper->RenderFrameToMainWindow(m_p2DVideoBuffer);
		}
		else
		{
			if (pFrame->isValid())
			{		
				frameDelay = m_pRenderWrapper->RenderFrameToMainWindow(pFrame);
				m_lastFrameTime = pFrame->time();
			}
		}	

		if (mEnv->m_bDebugDraw)
		{
			// Video information
			IRenderAuxText::Draw2dLabel(10.f, 10.0f, 1.5f, s_debugColorWhite, false, "Video file : %s.webm", m_p2DVideoPlayer->info().videoFileName.c_str());
			IRenderAuxText::Draw2dLabel(10.f, 25.0f, 1.5f, s_debugColorWhite, false, "Frame size : %dx%d", pFrame->displayWidth(), pFrame->displayHeight());
			IRenderAuxText::Draw2dLabel(10.f, 40.0f, 1.5f, s_debugColorWhite, false, "Frame rate : %d", (int)m_p2DVideoPlayer->info().frameRate);
			IRenderAuxText::Draw2dLabel(10.f, 55.0f, 1.5f, s_debugColorWhite, false, "Current state : %s", m_p2DVideoPlayer->getStateStr());
			IRenderAuxText::Draw2dLabel(10.f, 70.0f, 1.5f, s_debugColorWhite, false, "Current frame time : %0.3f / %0.3f", (float)pFrame->time(), m_p2DVideoPlayer->info().duration);

			if ((int)(frameDelay * 1000.0) > (int)(normalFrameDelay * 1000.0))
			{
				IRenderAuxText::Draw2dLabel(10.f, 85.0f, 1.5f, s_debugColorYellow, false, "Frame drawning speed : Slow (%d ms), Normal delay (%d ms)", (int)(frameDelay * 1000.0), (int)(normalFrameDelay * 1000.0));
			}
			else
			{
				IRenderAuxText::Draw2dLabel(10.f, 85.0f, 1.5f, s_debugColorWhite, false, "Frame drawning speed : Normal (%d ms)", (int)(frameDelay * 1000.0));
			}

			float currentFPS = gEnv->pTimer->GetFrameRate();
			IRenderAuxText::Draw2dLabel(10.f, 100.0f, 1.5f, s_debugColorWhite, false, "Current FPS (%d)", (int)currentFPS);

			// Audio information
			IRenderAuxText::Draw2dLabel(10.f, 115.0f, 1.5f, s_debugColorWhite, false, "Has audio track : %s", m_p2DVideoPlayer->info().hasAudio ? "true" : "false");
			IRenderAuxText::Draw2dLabel(10.f, 130.0f, 1.5f, s_debugColorWhite, false, "Audio channels : %d", m_p2DVideoPlayer->info().audioChannels);
			IRenderAuxText::Draw2dLabel(10.f, 145.0f, 1.5f, s_debugColorWhite, false, "Audio frequency : %d Hz", m_p2DVideoPlayer->info().audioFrequency);
			IRenderAuxText::Draw2dLabel(10.f, 160.0f, 1.5f, s_debugColorWhite, false, "Audio samples : %d", m_p2DVideoPlayer->info().audioSamples);	
		}
	}
	
	m_p2DVideoPlayer->frameBuffer()->unlockRead();
}

void C2DVideoQueue::ExecuteVideoEvent(EVideoPlayerEvents event)
{
	if (gEnv->pSystem->IsQuitting())
		return;

	for (const auto &it : m_Listeners)
	{
		it->OnVideoPlayerEvent(m_videoFileName.c_str(), event);
	}
}
