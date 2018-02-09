// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "TextureVideoQueue.h"
#include "Video/VideoPlayer.h"

#include "Render/Frame.h"
#include "Render/RenderWrapper.h"

#include "Video/VideoPlayer.h"

CTextureVideoQueue::CTextureVideoQueue()
	: m_pRenderWrapper(nullptr)
{
	if (gEnv->pGameFramework != nullptr)
	{
		gEnv->pGameFramework->RegisterListener(this, "CTextureVideoQueue_GameFrameworkListener", FRAMEWORKLISTENERPRIORITY_MENU);
	}

	if (gEnv->pSystem != nullptr)
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CTextureVideoQueue_Listener");
	}

	m_pRenderWrapper = new CRenderWrapper();
}

CTextureVideoQueue::~CTextureVideoQueue()
{
	SAFE_DELETE_11(m_pRenderWrapper);
}

void CTextureVideoQueue::Release()
{
	if (gEnv->pGameFramework != nullptr)
	{
		gEnv->pGameFramework->UnregisterListener(this);
	}

	if (gEnv->pSystem != nullptr)
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
	}

	Clear();
}


void CTextureVideoQueue::OnPostUpdate(float fDeltaTime)
{
	for (auto needDelete = m_Videos.begin(); needDelete != m_Videos.end();)
	{
		if (needDelete->m_bMarkForDelete)
		{
			SAFE_DELETE_11(needDelete->pVideoPlayer);

			if (needDelete->m_textureID)
			{
				gEnv->pRenderer->RemoveTexture(needDelete->m_textureID);
				needDelete->m_textureID = 0;
			}

			needDelete->m_bMarkForDelete = false;

			needDelete = m_Videos.erase(needDelete);
		}
		else
		{
			++needDelete;
		}
	}

	for (auto &it : m_Videos)
	{
		if (it.pVideoPlayer != nullptr && it.pVideoPlayer->isFinished())
		{
			it.pVideoPlayer->stop();
			it.m_bMarkForDelete = true;

			if (it.m_stopTextureName.empty())
			{
				CVideoFrame stopFrame(it.pVideoPlayer->info().width, it.pVideoPlayer->info().height);
				m_pRenderWrapper->UpdateTextureForTextureVideo(&stopFrame, it.m_textureID);
			}
			else
			{
				// TODO
				//ITexture* pTexture = gEnv->pRenderer->EF_LoadTexture(it.m_stopTextureName.c_str(), 0);
				//if (pTexture != nullptr)
				//{
				//	CVideoFrame stopFrame(it.pVideoPlayer->info().width, it.pVideoPlayer->info().height);
				//	pTexture->GetData32(0, 0, stopFrame.rgba());
				//	m_pRenderWrapper->UpdateTextureForTextureVideo(&stopFrame, it.m_textureID);
				//}
			}

			continue;
		}
		else if (it.pVideoPlayer != nullptr && it.pVideoPlayer->isNeedRestart())
		{
			it.pVideoPlayer->stop();
			it.pVideoPlayer->play();
			continue;
		}

		if (it.pVideoPlayer != nullptr)
		{
			CVideoFrame* pFrame = it.pVideoPlayer->frameBuffer()->lockRead();

			if (pFrame != nullptr)
			{
				if (!it.pVideoPlayer->isPaused())
				{
					if (pFrame->isValid() && it.m_textureID)
					{
						m_pRenderWrapper->UpdateTextureForTextureVideo(pFrame, it.m_textureID);
					}
				}			
			}

			it.pVideoPlayer->frameBuffer()->unlockRead();
			it.pVideoPlayer->update();
		}
	}
}

void CTextureVideoQueue::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
	case ESYSTEM_EVENT_LEVEL_UNLOAD:
		Clear();
		break;
	case ESYSTEM_EVENT_FULL_SHUTDOWN:
		Clear();
		break;
	case ESYSTEM_EVENT_FAST_SHUTDOWN:
		Clear();
		break;
	case ESYSTEM_EVENT_GAME_PAUSED:
		PauseAll(true);
		break;
	case ESYSTEM_EVENT_GAME_RESUMED:
		PauseAll(false);
		break;
	case ESYSTEM_EVENT_EDITOR_GAME_MODE_CHANGED:
	{
		if (wparam == 0) // Clear when exit from game mode
		{
			Clear();
		}
		break;
	}
	default:
		break;
	}
}

void CTextureVideoQueue::PlayVideo(IVideoPlayerEventListener* pListener, const char * videoName, const char * textureName, bool preload, bool looped, const char* stopTexture)
{
	LogDebug("<CTextureVideoQueue> PlayVideo(%p, %s, %s, %s, %s, %s)", pListener, videoName, textureName, 
		preload ? "true" : "false",
		looped ? "true" : "false",
		stopTexture);

	for (const auto &it : m_Videos)
	{
		if (it.m_textureName == textureName)
		{
			LogWarning("Can't play texture video because owner alredy play video");
			return;
		}
	}

	CVideoPlayer* pPlayer = new CVideoPlayer();

	SVideoPlayerResult result = pPlayer->load(videoName, preload, looped, -1, true, true);
	switch (result)
	{
	case SVideoPlayerResult::FileNotExists:
		LogError("Can't play texture video because video <%s> not exists", videoName);
		break;
	case SVideoPlayerResult::FailedParseHeader:
		LogError("Can't play texture video because can't parse header");
		break;
	case SVideoPlayerResult::FailedCreateInstance:
		LogError("Can't play texture video because failed create instance");
		break;
	case SVideoPlayerResult::FailedLoadSegment:
		LogError("Can't play texture video because failed load segment");
		break;
	case SVideoPlayerResult::FailedGetSegmentInfo:
		LogError("Can't play texture video because failed get segment info");
		break;
	case SVideoPlayerResult::UnsupportedVideoCodec:
		LogError("Can't play texture video - unsupported video codec");
		break;
	case SVideoPlayerResult::FailedInitializeVideoDecoder:
		LogError("Can't play texture video - failed init video decoder");
		break;
	case SVideoPlayerResult::Success:
	{
		int textureID = m_pRenderWrapper->CreateTextureForTextureVideo(pPlayer->info().width, pPlayer->info().height, textureName);
		if (textureID)
		{
			STextureVideo video;
			video.m_textureName = textureName;
			video.pVideoPlayer = pPlayer;
			video.m_textureID = textureID;
			video.m_stopTextureName = stopTexture;

			m_Videos.push_back(video);
		}
		else
		{
			LogError("Can't play texture video - error creation texture!");
		}

		if (pListener != nullptr)
		{
			pPlayer->registerEventListener(pListener);
		}

		pPlayer->play();
		return;
	}
	default:
		break;
	}

	SAFE_DELETE_11(pPlayer);
	return;
}

void CTextureVideoQueue::PauseVideo(const char* textureName)
{
	LogDebug("<CTextureVideoQueue> PauseVideo(%s)", textureName);

	for (const auto &it : m_Videos)
	{
		if (it.m_textureName == textureName)
		{
			if (it.pVideoPlayer != nullptr && it.pVideoPlayer->isPlaying())
			{
				it.pVideoPlayer->pause();
			}			
			return;
		}
	}
}

void CTextureVideoQueue::ResumeVideo(const char* textureName)
{
	LogDebug("<CTextureVideoQueue> ResumeVideo(%s)", textureName);

	for (const auto &it : m_Videos)
	{
		if (it.m_textureName == textureName)
		{
			if (it.pVideoPlayer != nullptr && it.pVideoPlayer->isPaused())
			{
				it.pVideoPlayer->play();
			}
			return;
		}
	}
}

void CTextureVideoQueue::StopVideo(const char* textureName)
{
	LogDebug("<CTextureVideoQueue> StopVideo(%s)", textureName);

	for (auto &it : m_Videos)
	{
		if (it.m_textureName == textureName)
		{
			if (it.pVideoPlayer != nullptr && !it.pVideoPlayer->isStopped())
			{
				it.pVideoPlayer->stop();
				it.m_bMarkForDelete = true;

				if (it.m_stopTextureName.empty())
				{
					CVideoFrame stopFrame(it.pVideoPlayer->info().width, it.pVideoPlayer->info().height);
					m_pRenderWrapper->UpdateTextureForTextureVideo(&stopFrame, it.m_textureID);
				}
			}
			return;
		}
	}
}

void CTextureVideoQueue::Clear()
{
	LogDebug("<CTextureVideoQueue> Clear()");

	for (auto it = m_Videos.begin(); it != m_Videos.end(); ++it)
	{
		if (it->pVideoPlayer != nullptr)
		{
			it->pVideoPlayer->stop();

			if (it->m_stopTextureName.empty())
			{
				CVideoFrame stopFrame(it->pVideoPlayer->info().width, it->pVideoPlayer->info().height);
				m_pRenderWrapper->UpdateTextureForTextureVideo(&stopFrame, it->m_textureID);
			}

			SAFE_DELETE_11(it->pVideoPlayer);
		}

		if (it->m_textureID)
		{
			gEnv->pRenderer->RemoveTexture(it->m_textureID);
			it->m_textureID = 0;
		}

		it->m_bMarkForDelete = false;
	}

	m_Videos.clear();
}

void CTextureVideoQueue::PauseAll(bool pause)
{
	LogDebug("<CTextureVideoQueue> PauseAll(%s)", pause ? "true" : "false");

	if (pause)
	{
		for (const auto &it : m_Videos)
		{
			if (it.pVideoPlayer != nullptr && it.pVideoPlayer->isPlaying())
			{
				it.pVideoPlayer->pause();
			}
		}
	}
	else
	{
		for (const auto &it : m_Videos)
		{
			if (it.pVideoPlayer != nullptr && it.pVideoPlayer->isPaused())
			{
				it.pVideoPlayer->play();
			}
		}
	}
}
