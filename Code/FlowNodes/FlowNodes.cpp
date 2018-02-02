// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlugin/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"

#include "Video/2DVideoQueue.h"
#include "Video/TextureVideoQueue.h"

#include <VideoPlugin/IVideoPluginListeners.h>

#include <CryFlowGraph/IFlowBaseNode.h>

class CFlowNode_2DVideoPlayer 
	: public CFlowBaseNode<eNCT_Singleton>
	, public IVideoPlayerEventListener
{
private:
	enum EInputPorts
	{
		eIP_Play = 0,
		eIP_Pause,
		eIP_Resume,
		eIP_Stop,
		eIP_VideoFileName,
		eIP_UsePreloadFile,
		eIP_LoopVideo,
		eIP_PlayInEditor,
		eIP_AudioTrack,
		eIP_IsSkippable,
		eIP_CanBePaused,
		eIP_IsLevelLoading,
	};

	enum EOutputPorts
	{
		eOP_StartPlay = 0,
		eOP_Paused,
		eOP_Resumed,
		eOP_Stopped,
	};

public:
	CFlowNode_2DVideoPlayer(SActivationInfo* pActInfo)
	{

	}
	~CFlowNode_2DVideoPlayer()
	{
		if (mEnv != nullptr && mEnv->pVideoQueue != nullptr)
		{
			mEnv->pVideoQueue->Unregister2DVideoPlayerListener(this);
		}
	}

	virtual void GetConfiguration(SFlowNodeConfig& config)
	{
		static const SInputPortConfig in_config[] = 
		{	
			InputPortConfig_Void("Play", _HELP("Start play video file")),
			InputPortConfig_Void("Pause", _HELP("Pause playing video file")),
			InputPortConfig_Void("Resume", _HELP("Resume paused video file")),
			InputPortConfig_Void("Stop", _HELP("Stop playing video file")),
			InputPortConfig<string>("Filename", _HELP("Video file name without extension and folder (e.g. logo)")),
			InputPortConfig<bool>("PreloadFile", false, _HELP("Use preloading file in memory before play")),
			InputPortConfig<bool>("LoopFile", false, _HELP("You can loop video file here")),
			InputPortConfig<bool>("PlayInEdidor", false, _HELP("Play video in editor")),
			InputPortConfig<int>("AudioTrackID", false, _HELP("You can switch audio tracks (e.g. for language change)")),
			InputPortConfig<bool>("IsSkippable", false, _HELP("This video can be skip")),
			InputPortConfig<bool>("CanBePaused", false, _HELP("This video can be paused by player")),
			//InputPortConfig<bool>("IsLevelLoading", false, _HELP("Setup it to true if video need play when level start loading")), // <-- TODO. OnPostUpdate function blocked when level loading, need different way for drawning
			{ 0 }
		};
		static const SOutputPortConfig out_config[] = 
		{
			OutputPortConfig_AnyType("StartPlay", _HELP("Executed when video file start playing (when video looped can be execute one more times)")),
			OutputPortConfig_AnyType("Paused", _HELP("Executed when video file paused")),
			OutputPortConfig_AnyType("Resumed", _HELP("Executed when video file resumed")),
			OutputPortConfig_AnyType("Stopped", _HELP("Executed when video file finish playing or when it stopped manually (when video looped can be execute one more times)")),
			{ 0 }
		};

		config.sDescription = _HELP("You can play 2D video files to viewport in .webm format from 'Videos' folder");
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo)
	{
		if(event == eFE_Activate)
		{
			m_pActInfo = *pActInfo;

			if (IsPortActive(pActInfo, eIP_Play))
			{
				if (mEnv == nullptr || mEnv->pVideoQueue == nullptr)
				{
					LogError("<CFlowNode_2DVideoPlayer> Can't play 2D video, because video queue == nullptr!");
					return;
				}

				string videoFileName = GetPortString(pActInfo, eIP_VideoFileName);
				bool preload = GetPortBool(pActInfo, eIP_UsePreloadFile);
				bool looped = GetPortBool(pActInfo, eIP_LoopVideo);
				bool playInEditor = GetPortBool(pActInfo, eIP_PlayInEditor);
				int audioTrackID = GetPortInt(pActInfo, eIP_AudioTrack);
				bool isSkippable = GetPortBool(pActInfo, eIP_IsSkippable);
				bool canBePaused = GetPortBool(pActInfo, eIP_CanBePaused);
				//bool isLevelLoading = GetPortBool(pActInfo, eIP_IsLevelLoading);

				if (gEnv->IsEditor() && !playInEditor)
				{
					LogError("<CFlowNode_2DVideoPlayer> Can't play 2D video, because playing blocking in editor!");
					return;
				}

				if (videoFileName.empty())
				{
					LogError("<CFlowNode_2DVideoPlayer> Can't play 2D video, because video file empty, check you flownodes logic and try again!");
					return;
				}
				else
				{
					mEnv->pVideoQueue->Play2DVideo(videoFileName, preload, looped, audioTrackID, isSkippable, canBePaused/*, isLevelLoading*/);
					mEnv->pVideoQueue->Register2DVideoPlayerListener(this);
				}
			}
			else if (IsPortActive(pActInfo, eIP_Pause))
			{
				if (mEnv != nullptr && mEnv->pVideoQueue != nullptr)
				{
					mEnv->pVideoQueue->Pause2DVideo();
				}
			}
			else if (IsPortActive(pActInfo, eIP_Resume))
			{
				if (mEnv != nullptr || mEnv->pVideoQueue != nullptr)
				{
					mEnv->pVideoQueue->Resume2DVideo();
				}
			}
			else if (IsPortActive(pActInfo, eIP_Stop))
			{
				if (mEnv != nullptr || mEnv->pVideoQueue != nullptr)
				{
					mEnv->pVideoQueue->Stop2DVideo();
				}
			}
		}
	}

	virtual void GetMemoryUsage(ICrySizer* s) const
	{
		s->Add(*this);
	}
public:
	void OnVideoPlayerEvent(EVideoPlayerEvents event) override
	{
		if (gEnv->pSystem != nullptr && gEnv->pSystem->IsQuitting())
		{
			return;
		}

		switch (event)
		{
		case EVideoPlayerEvents::OnPlay:
			ActivateOutput(&m_pActInfo, eOP_StartPlay, true);
			break;
		case EVideoPlayerEvents::OnPause:
			ActivateOutput(&m_pActInfo, eOP_Paused, true);
			break;
		case EVideoPlayerEvents::OnResume:
			ActivateOutput(&m_pActInfo, eOP_Resumed, true);
			break;
		case EVideoPlayerEvents::OnStop:
			ActivateOutput(&m_pActInfo, eOP_Stopped, true);
			break;
		default:
			break;
		}
	}
private:
	SActivationInfo m_pActInfo;
};

class CFlowNode_TextureVideoPlayer
	: public CFlowBaseNode<eNCT_Instanced>
	, public IVideoPlayerEventListener
{
private:
	enum EInputPorts
	{
		eIP_Play = 0,
		eIP_Pause,
		eIP_Resume,
		eIP_Stop,
		eIP_VideoFileName,
		eIP_OutTextureName,
//		eIP_StopTextureName, // <- TODO
		eIP_UsePreloadFile,
		eIP_LoopVideo,
		eIP_PlayInEditor,
	};

	enum EOutputPorts
	{
		eOP_StartPlay = 0,
		eOP_Paused,
		eOP_Resumed,
		eOP_Stopped,
	};
public:
	CFlowNode_TextureVideoPlayer(SActivationInfo* pActInfo)
	{

	}
	~CFlowNode_TextureVideoPlayer()
	{
	}

	virtual void GetConfiguration(SFlowNodeConfig& config) override
	{
		static const SInputPortConfig in_config[] =
		{
			InputPortConfig_Void("Play", _HELP("Start play video file to texture")),
			InputPortConfig_Void("Pause", _HELP("Pause playing video file")),
			InputPortConfig_Void("Resume", _HELP("Resume paused video file")),
			InputPortConfig_Void("Stop", _HELP("Stop playing video file")),
			InputPortConfig<string>("Filename", _HELP("Video file name without extension and folder (e.g. logo)")),
			InputPortConfig<string>("OutTextureName", _HELP("Output texture name with $ in begin. Example <$video_test>. Can't be null!")),
			//InputPortConfig<string>("StopTextureName", _HELP("This texture will be show when video stopped. Can be null")), <- TODO
			InputPortConfig<bool>("PreloadFile", false, _HELP("Use preloading file in memory before play")),
			InputPortConfig<bool>("LoopFile", false, _HELP("You can loop video file here")),
			InputPortConfig<bool>("PlayInEdidor", false, _HELP("Play video in editor")),
			{ 0 }
		};
		static const SOutputPortConfig out_config[] =
		{
			OutputPortConfig_AnyType("StartPlay", _HELP("Executed when video file start playing (when video looped can be execute one more times)")),
			OutputPortConfig_AnyType("Paused", _HELP("Executed when video file paused")),
			OutputPortConfig_AnyType("Resumed", _HELP("Executed when video file resumed")),
			OutputPortConfig_AnyType("Stopped", _HELP("Executed when video file finish playing or when it stopped manually (when video looped can be execute one more times)")),
			{ 0 }
		};

		config.sDescription = _HELP("You can play 2D video files to texture in .webm format from 'Videos' folder");
		config.pInputPorts = in_config;
		config.pOutputPorts = out_config;
		config.SetCategory(EFLN_APPROVED);
	}
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo) override
	{
		if (event == eFE_Activate)
		{
			m_pActInfo = *pActInfo;

			if (IsPortActive(pActInfo, eIP_Play))
			{
				if (mEnv->pTextureVideoQueue == nullptr)
				{
					LogError("<CFlowNode_TextureVideoPlayer> Can't play texture video, because texture queue == nullptr!");
					return;
				}

				string videoFileName = GetPortString(pActInfo, eIP_VideoFileName);
				string textureName = GetPortString(pActInfo, eIP_OutTextureName);
				//string stopTextureName = GetPortString(pActInfo, eIP_StopTextureName); // <- TODO
				bool preload = GetPortBool(pActInfo, eIP_UsePreloadFile);
				bool looped = GetPortBool(pActInfo, eIP_LoopVideo);
				bool playInEditor = GetPortBool(pActInfo, eIP_PlayInEditor);

				if (gEnv->IsEditor() && !playInEditor)
				{
					LogError("<CFlowNode_TextureVideoPlayer> Can't play texture video, because playing blocking in editor!");
					return;
				}

				if (videoFileName.empty())
				{
					LogError("<CFlowNode_TextureVideoPlayer> Can't play texture video, because video file empty, check you flownodes logic and try again!");
					return;
				}

				if (textureName.empty())
				{
					LogError("<CFlowNode_TextureVideoPlayer> Can't play texture video, because output texture name empty, check you flownodes logic and try again!");
					return;
				}
				if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
				{
					mEnv->pTextureVideoQueue->PlayVideo(this, videoFileName, textureName, preload, looped);
				}
			}
			else if (IsPortActive(pActInfo, eIP_Pause))
			{
				if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
				{
					string textureName = GetPortString(pActInfo, eIP_OutTextureName);
					mEnv->pTextureVideoQueue->PauseVideo(textureName);
				}
			}
			else if (IsPortActive(pActInfo, eIP_Resume))
			{
				if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
				{
					string textureName = GetPortString(pActInfo, eIP_OutTextureName);
					mEnv->pTextureVideoQueue->ResumeVideo(textureName);
				}
			}
			else if (IsPortActive(pActInfo, eIP_Stop))
			{
				if (mEnv != nullptr && mEnv->pTextureVideoQueue != nullptr)
				{
					string textureName = GetPortString(pActInfo, eIP_OutTextureName);
					mEnv->pTextureVideoQueue->StopVideo(textureName);
				}
			}
		}
	}

	virtual void GetMemoryUsage(ICrySizer* s) const override
	{
		s->Add(*this);
	}

	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo) override final
	{
		return new CFlowNode_TextureVideoPlayer(pActInfo);
	}
public:
	void OnVideoPlayerEvent(EVideoPlayerEvents event) override
	{
		if (gEnv->pSystem != nullptr && gEnv->pSystem->IsQuitting())
		{
			return;
		}

		switch (event)
		{
		case EVideoPlayerEvents::OnPlay:
			ActivateOutput(&m_pActInfo, eOP_StartPlay, true);
			break;
		case EVideoPlayerEvents::OnPause:
			ActivateOutput(&m_pActInfo, eOP_Paused, true);
			break;
		case EVideoPlayerEvents::OnResume:
			ActivateOutput(&m_pActInfo, eOP_Resumed, true);
			break;
		case EVideoPlayerEvents::OnStop:
			ActivateOutput(&m_pActInfo, eOP_Stopped, true);
			break;
		default:
			break;
		}
	}
private:
	SActivationInfo m_pActInfo;
};

REGISTER_FLOW_NODE("CryVideoPlugin:2DVideoPlayer", CFlowNode_2DVideoPlayer);
REGISTER_FLOW_NODE("CryVideoPlugin:TextureVideoPlayer", CFlowNode_TextureVideoPlayer);