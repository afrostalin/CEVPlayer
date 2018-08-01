// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "Plugin.h"

#if CRY_VERSION != 53
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/EnvPackage.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CrySchematyc/CoreAPI.h>
#include "Schematyc/TextureVideoPlayerComponent.h"
#else
USE_CRYPLUGIN_FLOWNODES
#endif

#include <CryCore/Platform/platform_impl.inl>

#include "Video/2DVideoQueue.h"
#include "Video/TextureVideoQueue.h"

#include "Input/InputDispatcher.h"

void CmdPlay2DVideo(IConsoleCmdArgs* args)
{
	if (args == nullptr)
		return;

	if (args->GetArgCount() > 1)
	{
		const char* video = args->GetArg(1);

		if (mEnv->pVideoQueue != nullptr)
		{
			mEnv->pVideoQueue->Play2DVideo(video);
		}
	}
}

void CmdPause2DVideo(IConsoleCmdArgs* args)
{
	if (mEnv->pVideoQueue != nullptr)
	{
		mEnv->pVideoQueue->Pause2DVideo();
	}
}

void CmdResume2DVideo(IConsoleCmdArgs* args)
{
	if (mEnv->pVideoQueue != nullptr)
	{
		mEnv->pVideoQueue->Resume2DVideo();
	}
}

void CmdStop2DVideo(IConsoleCmdArgs* args)
{
	if (mEnv->pVideoQueue != nullptr)
	{
		mEnv->pVideoQueue->Stop2DVideo();
	}
}

void CVideoPlugin::RegisterCVars()
{
	IConsole* pConsole = gEnv->pConsole;
	if (pConsole != nullptr && !gEnv->IsDedicated())
	{
		// Commands
		REGISTER_COMMAND("vp_play", CmdPlay2DVideo, VF_NULL, "Play video file. Ex. vp_play logo (without extension name)");
		REGISTER_COMMAND("vp_pause", CmdPause2DVideo, VF_NULL, "Pause current playing video");
		REGISTER_COMMAND("vp_resume", CmdResume2DVideo, VF_NULL, "Resume current paused video");
		REGISTER_COMMAND("vp_stop", CmdStop2DVideo, VF_NULL, "Stop current playing/paused video");

		// CVars
		REGISTER_CVAR2("vp_use_CrySDL", &mEnv->m_bUseCrySDL, 1, VF_NULL, "If you use CryAudioImplSDLMixer - Set it to 1, if not - 0");
		REGISTER_CVAR2("vp_debug_log", &mEnv->m_bDebugLog, 0, VF_NULL, "Video plugin debug log");
		REGISTER_CVAR2("vp_debug_draw", &mEnv->m_bDebugDraw, 0, VF_NULL, "Video plugin debug draw");
		REGISTER_CVAR2("vp_disableLogs", &mEnv->m_bDisableLog, 0, VF_NULL, "Disable logs from video plugin");
	}
}

void CVideoPlugin::UnRegisterCVars()
{
	IConsole* pConsole = gEnv->pConsole;
	if (pConsole != nullptr && !gEnv->IsDedicated())
	{
		pConsole->RemoveCommand("vp_play");
		pConsole->RemoveCommand("vp_pause");
		pConsole->RemoveCommand("vp_resume");
		pConsole->RemoveCommand("vp_stop");

		pConsole->UnregisterVariable("vp_use_CrySDL", true);
		pConsole->UnregisterVariable("vp_debug_log", true);
		pConsole->UnregisterVariable("vp_debug_draw", true);
		pConsole->UnregisterVariable("vp_disableLogs", true);
	}
}

CVideoPlugin::~CVideoPlugin()
{
	if (gEnv->pSystem != nullptr && !gEnv->IsDedicated())
	{
		gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
	}

#ifndef CRY_IS_MONOLITHIC_BUILD 
    #if CRY_VERSION != 53
	if (gEnv->pSchematyc != nullptr && !gEnv->IsDedicated())
	{
		gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(CVideoPlugin::GetCID());
	}
	#endif
#endif

	UnRegisterCVars();

	
	SAFE_RELEASE_11(mEnv->pVideoQueue);
	SAFE_RELEASE_11(mEnv->pTextureVideoQueue);
	SAFE_RELEASE_11(mEnv->pThreadManager);

	SAFE_DELETE_11(mEnv->pInputDispatcher);
	SAFE_DELETE_11(mEnv);
}

bool CVideoPlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	if (gEnv->pSystem != nullptr && !gEnv->IsDedicated())
	{
#if CRY_VERSION != 53
		gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CVideoPlugin_Listener");
#else
		gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);
#endif
	}

	if (!gEnv->IsDedicated())
	{
		mEnv->pThreadManager = new CVideoPluginThreadManager();
		mEnv->pThreadManager->SpawnDecoderThread();
	}

	if (gEnv->pConsole != nullptr)
	{
		ICVar* pStartScreen = gEnv->pConsole->GetCVar("sys_rendersplashscreen");
		if (pStartScreen != nullptr)
		{
			pStartScreen->Set(0);
		}
	}

	return true;
}

void CVideoPlugin::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	if (gEnv->IsDedicated())
	{
		return;
	}

	switch (event)
	{
	case ESYSTEM_EVENT_GAME_POST_INIT:
	{
		RegisterCVars();

		if (mEnv->pInputDispatcher == nullptr)
		{
			mEnv->pInputDispatcher = new CInputDispatcher();
		}

		if (mEnv->pVideoQueue == nullptr)
		{
			mEnv->pVideoQueue = new C2DVideoQueue();
		}

		if (mEnv->pTextureVideoQueue == nullptr)
		{
			mEnv->pTextureVideoQueue = new CTextureVideoQueue();
		}

		break;
	}
#if CRY_VERSION != 53
	case ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV:
	{
#ifndef CRY_IS_MONOLITHIC_BUILD 
		auto staticAutoRegisterLambda = [](Schematyc::IEnvRegistrar& registrar)
		{
			Detail::CStaticAutoRegistrar<Schematyc::IEnvRegistrar&>::InvokeStaticCallbacks(registrar);
		};

		if (gEnv->pSchematyc)
		{
			gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(
				stl::make_unique<Schematyc::CEnvPackage>(
					CVideoPlugin::GetCID(),
					"EntityComponents",
					"Ilya Chernetsov",
					"Components",
					staticAutoRegisterLambda
					)
			);
		}
#endif
		break;
	}
#endif
	default:
		break;
	}
}

void CVideoPlugin::PlayVideoToMainWindow(const char * videoName, bool preload, bool looped, int audioTrack, bool isSkippable, bool canBePaused, IVideoPlayerEventListener* pVideoPlayerListener)
{
	if (mEnv != nullptr && mEnv->pVideoQueue != nullptr)
	{
		mEnv->pVideoQueue->Play2DVideo(videoName, preload, looped, audioTrack, isSkippable, canBePaused);

		if (pVideoPlayerListener != nullptr)
		{
			mEnv->pVideoQueue->Register2DVideoPlayerListener(pVideoPlayerListener);
		}
	}
	else
	{
		LogError("<CVideoPlugin> Can't play 2D video video queue == nullptr!");
	}
}

void CVideoPlugin::PlayVideoToTexture(const char * videoName, const char * textureName, bool preload, bool looped, IVideoPlayerEventListener * pListener)
{
	if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr) 
	{
		mEnv->pTextureVideoQueue->PlayVideo(pListener, videoName, textureName, preload, looped);
	}
	else
	{
		LogError("<CVideoPlugin> Can't play texture video, because texture queue == nullptr!");
	}
}

void CVideoPlugin::PauseVideo(EVideoType videoType, const char* textureName)
{
	switch (videoType)
	{
	case EVideoType::ToMainWindow:
	{
		if (mEnv != nullptr && mEnv->pVideoQueue != nullptr)
		{
			mEnv->pVideoQueue->Pause2DVideo();
		}
		else
		{
			LogError("<CVideoPlugin> Can't pause 2D video video queue == nullptr!");
		}

		break;
	}
	case EVideoType::ToTexture:
	{
		if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
		{
			mEnv->pTextureVideoQueue->PauseVideo(textureName);
		}
		else
		{
			LogError("<CVideoPlugin> Can't pause texture video, because texture queue == nullptr!");
		}

		break;
	}
	default:
		break;
	}	
}

void CVideoPlugin::ResumeVideo(EVideoType videoType, const char* textureName)
{
	switch (videoType)
	{
	case EVideoType::ToMainWindow:
	{
		if (mEnv != nullptr && mEnv->pVideoQueue != nullptr)
		{
			mEnv->pVideoQueue->Resume2DVideo();
		}
		else
		{
			LogError("<CVideoPlugin> Can't resume 2D video video queue == nullptr!");
		}

		break;
	}
	case EVideoType::ToTexture:
	{
		if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
		{
			mEnv->pTextureVideoQueue->ResumeVideo(textureName);
		}
		else
		{
			LogError("<CVideoPlugin> Can't resume texture video, because texture queue == nullptr!");
		}

		break;
	}
	default:
		break;
	}
}

void CVideoPlugin::StopVideo(EVideoType videoType, const char* textureName)
{
	switch (videoType)
	{
	case EVideoType::ToMainWindow:
	{
		if (mEnv != nullptr && mEnv->pVideoQueue != nullptr)
		{
			mEnv->pVideoQueue->Stop2DVideo();
		}
		else
		{
			LogError("<CVideoPlugin> Can't stop 2D video video queue == nullptr!");
		}

		break;
	}
	case EVideoType::ToTexture:
	{
		if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
		{
			mEnv->pTextureVideoQueue->StopVideo(textureName);
		}
		else
		{
			LogError("<CVideoPlugin> Can't stop texture video, because texture queue == nullptr!");
		}

		break;
	}
	default:
		break;
	}
}

CRYREGISTER_SINGLETON_CLASS(CVideoPlugin)