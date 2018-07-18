// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "ThreadManager.h"

#include "DecoderThread.h"

#include <CryThreading/IThreadManager.h>
#include <CryThreading/IThreadConfigManager.h>

CVideoPluginThreadManager::CVideoPluginThreadManager()
	: m_DecoderThread(nullptr)
{
}

CVideoPluginThreadManager::~CVideoPluginThreadManager()
{
	SAFE_DELETE_11(m_DecoderThread);
}

void CVideoPluginThreadManager::SpawnDecoderThread()
{
	m_DecoderThread = new CDecoderThread();

	if (!gEnv->pThreadManager->SpawnThread(m_DecoderThread, "CryVideoPlugin_DecoderThread"))
	{
		SAFE_DELETE_11(m_DecoderThread);
		LogError("<CVideoPluginThreadManager> Can't spawn video decoder thread!");
	}
	else
	{
		LogDebug("<CVideoPluginThreadManager> Video decoder thread spawned");
	}
}

void CVideoPluginThreadManager::Release()
{
	if (m_DecoderThread != nullptr)
	{
		m_DecoderThread->SignalStopWork();

		if (gEnv->pThreadManager != nullptr)
		{
			gEnv->pThreadManager->JoinThread(m_DecoderThread, eJM_Join);
		}
	}
}

void CVideoPluginThreadManager::PushPlayer(CVideoPlayer * pPlayer)
{
	if(m_DecoderThread != nullptr)
	{
		m_DecoderThread->PushPlayer(pPlayer);
	}
}

void CVideoPluginThreadManager::RemovePlayer(CVideoPlayer * pPlayer)
{
	if(m_DecoderThread != nullptr)
	{
		m_DecoderThread->RemovePlayer(pPlayer);
	}
}
