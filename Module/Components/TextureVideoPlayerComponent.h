// Copyright (C) 2017-2020 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>

#include <CEVPlayer/IVideoPluginListeners.h>

namespace CEVPlayer
{
	class CTextureVideoPlayerComponent final
		: public IEntityComponent
		, public IVideoPlayerEventListener
	{
	public:
		CTextureVideoPlayerComponent() = default;
		virtual ~CTextureVideoPlayerComponent() {}
		static void ReflectType(Schematyc::CTypeDesc<CTextureVideoPlayerComponent>& desc);
		static void Register(Schematyc::CEnvRegistrationScope& componentScope);
	public:
		// IEntityComponent
		virtual void   Initialize() override;
		virtual void   OnShutDown() override;
		virtual Cry::Entity::EventFlags GetEventMask() const override;
		virtual void   ProcessEvent(const SEntityEvent& event) override;
		// ~IEntityComponent
	public:
		struct SOnStartPlaySignal
		{
			SOnStartPlaySignal() = default;
		};
		struct SOnPausedSignal
		{
			SOnPausedSignal() = default;
		};
		struct SOnResumedSignal
		{
			SOnResumedSignal() = default;
		};
		struct SOnStoppedSignal
		{
			SOnStoppedSignal() = default;
		};
	public:
		void Play(Schematyc::CSharedString videoFile, Schematyc::CSharedString outputTexture, bool preload, bool looped, bool canPlayInEditor);
		void Pause();
		void Resume();
		void Stop();
	public:
		// IVideoPlayerEventListener
		virtual void OnVideoPlayerEvent(const char* videoFileName, EVideoPlayerEvents event) override;
		// ~IVideoPlayerEventListener
	public:
		bool                     m_IsAutoPlay = false;
		Schematyc::CSharedString m_VideoFileName;
		Schematyc::CSharedString m_OutputFileName;
		bool                     m_IsPreloaded = true;
		bool                     m_IsLooped = false;
		bool                     m_IsCanPlayInEditor = false;
	};
}