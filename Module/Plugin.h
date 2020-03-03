// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CryEntitySystem/IEntityClass.h>
#include <CEVPlayer/IVideoPlugin.h>

namespace CEVPlayer
{
	class CVideoPlugin
		: public IVideoPlugin
		, public ISystemEventListener
	{
	public:
		CRYINTERFACE_BEGIN()
			CRYINTERFACE_ADD(IVideoPlugin)
			CRYINTERFACE_ADD(Cry::IEnginePlugin)
			CRYINTERFACE_END()

			CRYGENERATE_SINGLETONCLASS_GUID(CVideoPlugin, "CEVPlayer", "{C14CBDCB-1CFF-41B9-A220-E16E19A21096}"_cry_guid)

			PLUGIN_FLOWNODE_REGISTER;
		PLUGIN_FLOWNODE_UNREGISTER;

		virtual ~CVideoPlugin();
	public:
		// ICryPlugin
		virtual const char* GetName() const override { return "CEVPlayer"; }
		virtual const char* GetCategory() const override { return "Render"; }
		virtual bool        Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
		// ~ICryPlugin

		// ISystemEventListener
		virtual void        OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
		// ~ISystemEventListener

		// IVideoPlugin
		virtual void PlayVideoToMainWindow(const char* videoName, bool preload = false, bool looped = false, int audioTrack = 0,
			bool isSkippable = false, bool canBePaused = false, IVideoPlayerEventListener * pVideoPlayerListener = nullptr) override;
		virtual void PlayVideoToTexture(const char* videoName, const char* textureName, bool preload = false, bool looped = false, IVideoPlayerEventListener* pListener = nullptr) override;
		virtual void PauseVideo(EVideoType videoType, const char* textureName = nullptr) override;
		virtual void ResumeVideo(EVideoType videoType, const char* textureName = nullptr) override;
		virtual void StopVideo(EVideoType videoType, const char* textureName = nullptr) override;
		// ~IVideoPlugin
	public:
		void        RegisterCVars();
		void        UnRegisterCVars();
	protected:
		void        RegisterComponents(Schematyc::IEnvRegistrar& registrar);
	};
}