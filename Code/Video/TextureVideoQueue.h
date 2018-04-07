// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CryGame/IGameFramework.h>
#include <CryInput/IInput.h>

#include "Video/VideoPlayer.h"

class CRenderWrapper;
class CInputDispatcher;
class CVideoPlayer;
struct IVideoPlayerEventListener;

struct STextureVideo
{
	string                     m_textureName;
	string                     m_stopTextureName;
	CVideoPlayer*              pVideoPlayer = nullptr;
	int                        m_textureID = 0;
	bool                       m_bMarkForDelete = false;
};

class CTextureVideoQueue 
	: public IGameFrameworkListener
	, public ISystemEventListener
{
public:
	CTextureVideoQueue();
	~CTextureVideoQueue();
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
	void              PlayVideo(IVideoPlayerEventListener* pListener, const char* videoName, const char* textureName, bool preload = false, bool looped = false, const char* stopTexture = nullptr);
	void              PauseVideo(const char* textureName);
	void              ResumeVideo(const char* textureName);
	void              StopVideo(const char* textureName);
private:
	void              Clear();
	void              PauseAll(bool pause);
private:
	CRenderWrapper*            m_pRenderWrapper;
	std::vector<STextureVideo> m_Videos;
};