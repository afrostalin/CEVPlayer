// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlugin/blob/master/LICENSE

#pragma once

#include <CryEntitySystem/IEntityClass.h>

#include <VideoPlugin/IVideoPlugin.h>

class CVideoPlugin
	: public IVideoPlugin
	, public ISystemEventListener
{
public:
	CRYINTERFACE_BEGIN()
	CRYINTERFACE_ADD(IVideoPlugin)
	CRYINTERFACE_ADD(ICryPlugin)
	CRYINTERFACE_END()

	CRYGENERATE_SINGLETONCLASS_GUID(CVideoPlugin, "CryVideoPlugin", "093DE361-69A6-41FE-9794-068A2234A421"_cry_guid)

	PLUGIN_FLOWNODE_REGISTER;
	PLUGIN_FLOWNODE_UNREGISTER;

	virtual ~CVideoPlugin();
public:
	// ICryPlugin
	const char* GetName() const override { return "CryVideoPlugin"; }
	const char* GetCategory() const override { return "Render"; }
	bool        Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	void        OnPluginUpdate(EPluginUpdateType updateType) override {};
	// ~ICryPlugin

	// ISystemEventListener
	void        OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// IVideoPlugin
	void        Play2DVideo(const char * videoName, bool preload = false, bool looped = false, int audioTrack = 0, bool isSkippable = false, bool canBePaused = false, IVideoPlayerEventListener* pVideoPlayerListener = nullptr) override;
	void        Pause2DVideo() override;
	void        Resume2DVideo() override;
	void        Stop2DVideo() override;
	bool        Is2DVideoCurrentlyPlaying() override;
	// ~IVideoPlugin
public:
	void        RegisterCVars();
	void        UnRegisterCVars();
};