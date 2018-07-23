// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

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
#if CRY_VERSION == 53 || CRY_VERSION == 54
	CRYINTERFACE_ADD(ICryPlugin)
#elif CRY_VERSION == 55
	CRYINTERFACE_ADD(Cry::IEnginePlugin)
#endif
	CRYINTERFACE_END()

#if CRY_VERSION != 53
	CRYGENERATE_SINGLETONCLASS_GUID(CVideoPlugin, "CEVPlayer", "093DE361-69A6-41FE-9794-068A2234A421"_cry_guid)
#else
	CRYGENERATE_SINGLETONCLASS(CVideoPlugin, "CEVPlayer", 0x093DE36169A641FE, 0x9794068A2234A421);
#endif

	PLUGIN_FLOWNODE_REGISTER;
	PLUGIN_FLOWNODE_UNREGISTER;

	virtual ~CVideoPlugin();
public:
	// ICryPlugin
	virtual const char* GetName() const override { return "CEVPlayer"; }
	virtual const char* GetCategory() const override { return "Render"; }
	virtual bool        Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
#if CRY_VERSION == 53 || CRY_VERSION == 54
	virtual void        OnPluginUpdate(EPluginUpdateType updateType) override {};
#endif
	// ~ICryPlugin

	// ISystemEventListener
	virtual void        OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// IVideoPlugin
	virtual void        Play2DVideo(const char * videoName, bool preload = false, bool looped = false, int audioTrack = 0, bool isSkippable = false, bool canBePaused = false, IVideoPlayerEventListener* pVideoPlayerListener = nullptr) override;
	virtual void        Pause2DVideo() override;
	virtual void        Resume2DVideo() override;
	virtual void        Stop2DVideo() override;
	virtual bool        Is2DVideoCurrentlyPlaying() override;
	// ~IVideoPlugin
public:
	void        RegisterCVars();
	void        UnRegisterCVars();
};