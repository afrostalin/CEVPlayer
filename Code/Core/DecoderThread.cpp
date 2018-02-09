// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "DecoderThread.h"

#include "Video/VideoPlayer.h"

#include "Render/Frame.h"
#include "Render/RenderWrapper.h"

#include "Video/2DVideoQueue.h"

void CDecoderThread::ThreadEntry()
{
	while (!bIsReadyToClose && !gEnv->pSystem->IsQuitting())
	{
		m_Mutex.Lock();
		DecodeProcess();
		m_Mutex.Unlock();
	}

	m_Players.clear();
}

void CDecoderThread::PushPlayer(CVideoPlayer * pPlayer)
{
	m_Mutex.Lock();
	m_Players.push_back(pPlayer);
	m_Mutex.Unlock();
}

void CDecoderThread::RemovePlayer(CVideoPlayer * pPlayer)
{
	m_Mutex.Lock();
	for (auto it = m_Players.begin(); it != m_Players.end(); ++it)
	{
		if (*it == pPlayer)
		{
			m_Players.erase(it);
			break;
		}
	}
	m_Mutex.Unlock();
}

void CDecoderThread::DecodeProcess()
{
	if ((gEnv->pSystem != nullptr && gEnv->pSystem->IsQuitting()) || gEnv->pSystem == nullptr || m_Players.empty())
	{
		return;
	}

	for (auto it : m_Players)
	{
		if (it != nullptr && (it->isPlaying() || it->isBuffering()))
		{
			it->decode();
		}	
	}
}
