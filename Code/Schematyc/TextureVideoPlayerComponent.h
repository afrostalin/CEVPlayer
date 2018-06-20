// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntity.h>

#include <VideoPlugin/IVideoPluginListeners.h>

#ifndef CRY_IS_MONOLITHIC_BUILD

class CTextureVideoPlayerComponent final 
	: public IEntityComponent
	, public IVideoPlayerEventListener
{
public:
	CTextureVideoPlayerComponent() = default;
	virtual ~CTextureVideoPlayerComponent() {}
	static void ReflectType(Schematyc::CTypeDesc<CTextureVideoPlayerComponent>& desc);
public:
	// IEntityComponent
	virtual void   Initialize() override;
	virtual void   OnShutDown() override;
	virtual uint64 GetEventMask() const override;
#if CRY_VERSION == 53 || CRY_VERSION == 54
	virtual void   ProcessEvent(SEntityEvent& event) override;
#elif CRY_VERSION == 55
	virtual void   ProcessEvent(const SEntityEvent& event) override;
#endif
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
	virtual void OnVideoPlayerEvent(EVideoPlayerEvents event) override;
	// ~IVideoPlayerEventListener
public:
	bool                     m_IsAutoPlay = false;
	Schematyc::CSharedString m_VideoFileName;
	Schematyc::CSharedString m_OutputFileName;
	bool                     m_IsPreloaded = true;
	bool                     m_IsLooped = false;
	bool                     m_IsCanPlayInEditor = false;
};

#endif