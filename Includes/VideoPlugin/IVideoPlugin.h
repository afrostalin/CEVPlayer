// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CrySystem/ICryPlugin.h>
#include "IVideoPluginListeners.h"

enum class EVideoType : int
{
	ToMainWindow = 0,
	ToTexture,
};

struct IVideoPlugin 
#if CRY_VERSION == 53 || CRY_VERSION == 54
	: public ICryPlugin
#elif CRY_VERSION == 55
	: public Cry::IEnginePlugin
#endif
{
#if CRY_VERSION != 53
	CRYINTERFACE_DECLARE_GUID(IVideoPlugin, "AAB3D63C-77A4-482A-8D25-3E362B906321"_cry_guid);
#else
	CRYINTERFACE_DECLARE(IVideoPlugin, 0xAAB3D63C77A4482A, 0x8D253E362B906321);
#endif 
public:
	//! Stream video to main window
	virtual void PlayVideoToMainWindow(const char * videoName, bool preload = false, bool looped = false, int audioTrack = 0,
		bool isSkippable = false, bool canBePaused = false, IVideoPlayerEventListener* pVideoPlayerListener = nullptr) = 0;
	//! Stream video to texture only
	virtual void PlayVideoToTexture(const char* videoName, const char* textureName, bool preload = false, bool looped = false, IVideoPlayerEventListener* pListener = nullptr) = 0;

	//! Pause video - textureName argument used only for pausing video streamed to texture
	virtual void PauseVideo(EVideoType videoType, const char* textureName = nullptr) = 0;

	//! Resume video - textureName argument used only for pausing video streamed to texture
	virtual void ResumeVideo(EVideoType videoType, const char* textureName = nullptr) = 0;

	//! Stop video - textureName argument used only for pausing video streamed to texture
	virtual void StopVideo(EVideoType videoType, const char* textureName = nullptr) = 0;
};