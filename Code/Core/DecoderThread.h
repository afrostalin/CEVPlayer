// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlugin/blob/master/LICENSE

#pragma once

#include <CryThreading/IThreadManager.h>


class CVideoPlayer;

class CDecoderThread : public IThread
{
public:
	CDecoderThread() : bIsReadyToClose(false)
	{}
	~CDecoderThread() {}
public:
	virtual void ThreadEntry();
	void         SignalStopWork() { bIsReadyToClose = true; }
	bool         IsReadyToClose() { return bIsReadyToClose; }
public:
	void         PushPlayer(CVideoPlayer* pPlayer);
	void         RemovePlayer(CVideoPlayer* pPlayer);
private:
	void         DecodeProcess();
private:
	bool                       bIsReadyToClose;
	CryMutex                   m_Mutex;
	std::vector<CVideoPlayer*> m_Players;
};