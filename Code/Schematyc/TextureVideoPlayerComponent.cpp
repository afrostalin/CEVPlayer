// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlugin/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "TextureVideoPlayerComponent.h"
#include "Video/TextureVideoQueue.h"

#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/IObject.h>

#ifndef CRY_IS_MONOLITHIC_BUILD // WHY THIS NOT WORK IN STATIC MODE, CRYTEK ?

static void ReflectType(Schematyc::CTypeDesc<CTextureVideoPlayerComponent::SOnStartPlaySignal>& desc)
{
	desc.SetGUID("672B590C-E387-4357-BDD0-A3C3884277C5"_cry_guid);
	desc.SetLabel("OnStartPlay");
	desc.SetDescription("Executed when video start playing to texture");
}

static void ReflectType(Schematyc::CTypeDesc<CTextureVideoPlayerComponent::SOnPausedSignal>& desc)
{
	desc.SetGUID("92C48065-FC02-4CD3-A609-AF6384498627"_cry_guid);
	desc.SetLabel("OnPaused");
	desc.SetDescription("Executed when video paused");
}

static void ReflectType(Schematyc::CTypeDesc<CTextureVideoPlayerComponent::SOnResumedSignal>& desc)
{
	desc.SetGUID("176C5938-389B-470F-886D-CD13A82D9744"_cry_guid);
	desc.SetLabel("OnResumed");
	desc.SetDescription("Executed when video resumed");
}

static void ReflectType(Schematyc::CTypeDesc<CTextureVideoPlayerComponent::SOnStoppedSignal>& desc)
{
	desc.SetGUID("7F0EFF91-47B4-45F1-982E-F15613C9D4FB"_cry_guid);
	desc.SetLabel("OnStopped");
	desc.SetDescription("Executed when video stopped");
}

static void RegisterSpawnPointComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CTextureVideoPlayerComponent));
		{
			{
				auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTextureVideoPlayerComponent::Play, "1240FF98-3145-48BD-B282-4AEC7D647C1D"_cry_guid, "Play");
				pFunction->SetDescription("Start play video to texture");
				pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
				pFunction->BindInput(1, 'val1', "VideoFilename", "Video file name without folder and extension (e.g. logo)");
				pFunction->BindInput(2, 'val2', "OutputTextureName", "Output texture name for using in materials. Can't be null");
				pFunction->BindInput(3, 'val3', "IsPreloaded", "File will be preloaded in memory before playing");
				pFunction->BindInput(4, 'val4', "IsLooped", "Video can be looped here");
				pFunction->BindInput(5, 'val5', "CanPlayInEditor", "Video can be play in editor");
				componentScope.Register(pFunction);
			}

			{
				auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTextureVideoPlayerComponent::Pause, "33121C57-8DDE-4845-9357-F4E34F6E77C9"_cry_guid, "Pause");
				pFunction->SetDescription("Pause current playing video");
				pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
				componentScope.Register(pFunction);
			}

			{
				auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTextureVideoPlayerComponent::Resume, "B6D31737-C79E-4A36-874D-A685A7C6E9FC"_cry_guid, "Resume");
				pFunction->SetDescription("Resume paused video");
				pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
				componentScope.Register(pFunction);
			}

			{
				auto pFunction = SCHEMATYC_MAKE_ENV_FUNCTION(&CTextureVideoPlayerComponent::Stop, "1EA6B0BD-4ED5-4C66-9AE0-1CC92DAD6AF7"_cry_guid, "Stop");
				pFunction->SetDescription("Stop playing video");
				pFunction->SetFlags(Schematyc::EEnvFunctionFlags::Construction);
				componentScope.Register(pFunction);
			}

			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CTextureVideoPlayerComponent::SOnStartPlaySignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CTextureVideoPlayerComponent::SOnPausedSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CTextureVideoPlayerComponent::SOnResumedSignal));
			componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CTextureVideoPlayerComponent::SOnStoppedSignal));
		}
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSpawnPointComponent)

void CTextureVideoPlayerComponent::ReflectType(Schematyc::CTypeDesc<CTextureVideoPlayerComponent>& desc)
{
	desc.SetGUID("1AE1AB31-50A4-4D83-BAF0-FFB48CABF0B2"_cry_guid);
	desc.SetEditorCategory("Video");
	desc.SetLabel("TextureVideoPlayer");
	desc.SetDescription("Play video to texture");
	desc.SetIcon("icons:General/Camera.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::ClientOnly });

	desc.AddMember(&CTextureVideoPlayerComponent::m_IsAutoPlay, 'auto', "autoplay", "IsAutoPlay", "If set it to true - video start play when game started", false);
	desc.AddMember(&CTextureVideoPlayerComponent::m_VideoFileName, 'vide', "videofile", "VideoFileName", "Video file name witout folder and extension", "");
	desc.AddMember(&CTextureVideoPlayerComponent::m_OutputFileName, 'text', "outtexture", "OutputTextureName", "Output texture name for using in materials. Can't be null. Need start for $ simbol. (e.g. $videoplayer_1)", "");
	desc.AddMember(&CTextureVideoPlayerComponent::m_IsPreloaded, 'prel', "preloaded", "IsPreloaded", "If set it to true - before playing video his will be loaded in memory", true);
	desc.AddMember(&CTextureVideoPlayerComponent::m_IsLooped, 'loop', "looped", "IsLooped", "If set it to true - video will be looped", false);
	desc.AddMember(&CTextureVideoPlayerComponent::m_IsCanPlayInEditor, 'edit', "canplayineditor", "IsCanPlayInEditor", "If set it to true - video can be played in editor mode", false);
}

void CTextureVideoPlayerComponent::Initialize()
{
}

void CTextureVideoPlayerComponent::OnShutDown()
{
	Stop();
}

uint64 CTextureVideoPlayerComponent::GetEventMask() const
{
	return ENTITY_EVENT_BIT(ENTITY_EVENT_START_GAME);
}

void CTextureVideoPlayerComponent::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_START_GAME:
	{
		if (m_IsAutoPlay)
		{
			Play(m_VideoFileName, m_OutputFileName, m_IsPreloaded, m_IsLooped, m_IsCanPlayInEditor);
		}

		break;
	}
	default:
		break;
	}
}

void CTextureVideoPlayerComponent::Play(Schematyc::CSharedString videoFile, Schematyc::CSharedString outputTexture, bool preload, bool looped, bool canPlayInEditor)
{
	if (videoFile.empty())
	{
		LogError("<CTextureVideoPlayerComponent> Can't play video, because video file name empty, check you component settings and try again!");
		return;
	}

	if (outputTexture.empty())
	{
		LogError("<CTextureVideoPlayerComponent> Can't play video, because output texture name empty, check you component settings and try again!");
		return;
	}

	if (gEnv->IsEditor() && !canPlayInEditor)
	{
		LogError("<CTextureVideoPlayerComponent> Can't play video, because playing blocking in editor mode!");
		return;
	}

	if (!m_IsAutoPlay)
	{
		m_VideoFileName = videoFile;
		m_OutputFileName = outputTexture;
		m_IsPreloaded = preload;
		m_IsLooped = looped;
		m_IsCanPlayInEditor = canPlayInEditor;
	}

	if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
	{
		mEnv->pTextureVideoQueue->PlayVideo(this, videoFile.c_str(), outputTexture.c_str(), preload, looped);
	}
}

void CTextureVideoPlayerComponent::Pause()
{
	if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
	{
		mEnv->pTextureVideoQueue->PauseVideo(m_OutputFileName.c_str());
	}
}

void CTextureVideoPlayerComponent::Resume()
{
	if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
	{
		mEnv->pTextureVideoQueue->ResumeVideo(m_OutputFileName.c_str());
	}
}

void CTextureVideoPlayerComponent::Stop()
{
	if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
	{
		mEnv->pTextureVideoQueue->StopVideo(m_OutputFileName.c_str());
	}
}

void CTextureVideoPlayerComponent::OnVideoPlayerEvent(EVideoPlayerEvents event)
{
	if (gEnv->pSystem != nullptr && gEnv->pSystem->IsQuitting())
	{
		return;
	}

	Schematyc::IObject* const pSchematycObject = m_pEntity->GetSchematycObject();

	if (pSchematycObject == nullptr)
	{
		return;
	}

	switch (event)
	{
	case EVideoPlayerEvents::OnPlay:
		pSchematycObject->ProcessSignal(SOnStartPlaySignal(), GetGUID());
		break;
	case EVideoPlayerEvents::OnPause:
		pSchematycObject->ProcessSignal(SOnPausedSignal(), GetGUID());
		break;
	case EVideoPlayerEvents::OnResume:
		pSchematycObject->ProcessSignal(SOnResumedSignal(), GetGUID());
		break;
	case EVideoPlayerEvents::OnStop:
		pSchematycObject->ProcessSignal(SOnStoppedSignal(), GetGUID());
		break;
	default:
		break;
	}
}

#endif