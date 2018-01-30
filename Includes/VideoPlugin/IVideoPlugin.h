// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlugin/blob/master/LICENSE

#pragma once

#include <CrySystem/ICryPlugin.h>
#include "IVideoPluginListeners.h"

#define USE_DEPRICATED_CRYENGINE_MACROS 0

struct IVideoPlugin : public ICryPlugin
{
#if !USE_DEPRICATED_CRYENGINE_MACROS
	CRYINTERFACE_DECLARE_GUID(IVideoPlugin, "AAB3D63C-77A4-482A-8D25-3E362B906321"_cry_guid);
#else
	CRYINTERFACE_DECLARE(IVideoPlugin, 0xAAB3D63C77A4482A, 0x8D253E362B906321);
#endif 
public:
	virtual void Play2DVideo(const char * videoName, bool preload = false, bool looped = false, int audioTrack = 0,
		bool isSkippable = false, bool canBePaused = false, IVideoPlayerEventListener* pVideoPlayerListener = nullptr) = 0;
	virtual void Pause2DVideo() = 0;
	virtual void Resume2DVideo() = 0;
	virtual void Stop2DVideo() = 0;
	virtual bool Is2DVideoCurrentlyPlaying() = 0;
};