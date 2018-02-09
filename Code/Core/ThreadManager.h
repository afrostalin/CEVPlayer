// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlayer/blob/master/LICENSE

#pragma once

class CDecoderThread;
class CVideoPlayer;

class CVideoPluginThreadManager
{
public:
	CVideoPluginThreadManager();
	~CVideoPluginThreadManager();
public:
	void SpawnDecoderThread();
	void Release();
public:
	void PushPlayer(CVideoPlayer* pPlayer);
	void RemovePlayer(CVideoPlayer* pPlayer);
private:
	CDecoderThread* m_DecoderThread;
};