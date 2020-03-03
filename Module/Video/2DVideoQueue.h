// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CryGame/IGameFramework.h>
#include <CryInput/IInput.h>

#include "Video/VideoPlayer.h"

namespace CEVPlayer
{
	class CRenderWrapper;
	class CVideoFrame;
	class CInputDispatcher;

	struct SQueuedVideo
	{
		IVideoPlayerEventListener* pListener = nullptr;
		std::string videoName;
		bool preload = false;
		bool looped = false;
		int audioTrack = 0;
		bool isSkippable = false;
		bool canBePaused = false;
		bool isLevelLoading = false;
	};

	class C2DVideoQueue
		: public IGameFrameworkListener
		, public ISystemEventListener
	{
	public:
		C2DVideoQueue();
		~C2DVideoQueue();
	public:
		// IGameFrameworkListener
		void              OnPostUpdate(float fDeltaTime) override;
		void              OnSaveGame(ISaveGame* pSaveGame) override {};
		void              OnLoadGame(ILoadGame* pLoadGame) override {};
		void              OnLevelEnd(const char* nextLevel) override {};
		void              OnActionEvent(const SActionEvent& event) override {};
		void              OnPreRender() override {};
		//~ IGameFrameworkListener

		// ISystemEventListener
		void              OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
		// ~ISystemEventListener
	public:
		void              Release();
	public:
		void              Play2DVideo(IVideoPlayerEventListener* pListener, const char* videoName, bool preload = false, bool looped = false, int audioTrack = 0, bool isSkippable = false, bool canBePaused = false, bool isLevelLoading = false);
		void              Pause2DVideo();
		void              Resume2DVideo();
		void              Stop2DVideo();
		bool              Is2DVideoPlayingNow();
	public:
		void              OnActionPause2DVideo(EInputState activationMode, float value);
		void              OnActionSkip2DVideo(EInputState activationMode, float value);
	public:
		void              RemoveVideoListener(IVideoPlayerEventListener* pListener);
	private:
		CVideoPlayer* m_p2DVideoPlayer;
		CVideoFrame* m_p2DVideoBuffer;
	private:
		void              Draw2DVideo(float fDeltaTime);
	private:
		CRenderWrapper* m_pRenderWrapper;
		double            m_lastFrameTime;
	private:
		std::vector<IVideoPlayerEventListener*> m_Listeners;
		std::string       m_videoFileName;
		std::queue<SQueuedVideo> m_queuedVideos;
	};
}