// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CrySystem/ICryPlugin.h>
#include "IVideoPluginListeners.h"

enum class EVideoType : int
{
	ToMainWindow = 0,
	ToTexture,
};

struct IVideoPlugin : public Cry::IEnginePlugin
{
	CRYINTERFACE_DECLARE_GUID(IVideoPlugin, "{4DCA4F01-AE95-4735-AADA-50E270418BB8}"_cry_guid);
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

	//! Add video player listener manualy if it need
	virtual void AddListener(IVideoPlayerEventListener* pVideoPlayerListener) = 0;

	//! Execute this when you need remove event listener
	virtual void RemoveListener(IVideoPlayerEventListener* pVideoPlayerListener) = 0;
};