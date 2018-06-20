// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "RenderWrapper.h"
#include "Frame.h"

#define VIDEO_PLAYER_2D_TEXTURE "$video_player_2D"

CRenderWrapper::~CRenderWrapper()
{
	Release2DVideoTextures();
}

void CRenderWrapper::Create2DVideoTextures()
{
	if (gEnv->pRenderer == nullptr)
	{
		LogError("<CRenderWrapper> Can't create 2D video texture - render nullptr");
		return;
	}

	const int screenWidth = gEnv->pRenderer->GetOverlayWidth();
	const int screenHeight = gEnv->pRenderer->GetOverlayHeight();

#if CRY_VERSION == 53 || CRY_VERSION == 54
	m_2DTextureID = gEnv->pRenderer->DownLoadToVideoMemory(nullptr, screenWidth, screenHeight, eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, VIDEO_PLAYER_2D_TEXTURE, FT_NOMIPS);
#elif CRY_VERSION == 55
	m_2DTextureID = gEnv->pRenderer->UploadToVideoMemory(nullptr, screenWidth, screenHeight, eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, VIDEO_PLAYER_2D_TEXTURE, FT_NOMIPS);
#endif

	LogDebug("<CRenderWrapper> Create 2D texture with name <" VIDEO_PLAYER_2D_TEXTURE "> id <%d> resolution <%dx%d>", m_2DTextureID, screenWidth, screenHeight);
}

double CRenderWrapper::RenderFrameToMainWindow(CVideoFrame* pFrame)
{
	if ((pFrame == nullptr)
		|| (pFrame != nullptr && (pFrame->displayWidth() <= 0 || pFrame->displayHeight() <= 0))
		|| (gEnv->pRenderer == nullptr))
	{
		return -1.0;
	}
#ifdef DEBUG
	LARGE_INTEGER t1, t2, f;
	QueryPerformanceCounter(&t1);
#endif

	if (m_2DTextureID == 0)
	{
#if CRY_VERSION == 53 || CRY_VERSION == 54
		m_2DTextureID = gEnv->pRenderer->DownLoadToVideoMemory(nullptr, pFrame->displayWidth(), pFrame->displayHeight(), eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, VIDEO_PLAYER_2D_TEXTURE, FT_NOMIPS);
#elif CRY_VERSION == 55
		m_2DTextureID = gEnv->pRenderer->UploadToVideoMemory(nullptr, pFrame->displayWidth(), pFrame->displayHeight(), eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, VIDEO_PLAYER_2D_TEXTURE, FT_NOMIPS);
#endif
		LogDebug("<CRenderWrapper> Create 2D texture with name <" VIDEO_PLAYER_2D_TEXTURE "> id <%d> resolution <%dx%d>", m_2DTextureID, pFrame->displayWidth(), pFrame->displayHeight());
	}

	if (m_2DTextureID > 0)
	{		
		ITexture* pTexture = gEnv->pRenderer->EF_GetTextureByID(m_2DTextureID);

		if (pTexture != nullptr)
		{
			gEnv->pRenderer->UpdateTextureInVideoMemory(m_2DTextureID, pFrame->rgba(), 0, 0, pFrame->displayWidth(), pFrame->displayHeight(), eTF_R8G8B8A8);
		}
		else
		{
			LogError("<CRenderWrapper> Can't update texture with id <%d> - Texture not found ! Potential problem in releasing old texture!", m_2DTextureID);
		}

		Draw2dImage(pTexture);	
	}
#ifdef DEBUG
	QueryPerformanceCounter(&t2);
	QueryPerformanceFrequency(&f);
	return double(t2.QuadPart - t1.QuadPart) / f.QuadPart;
#else 
	return -1.0;
#endif
}

void CRenderWrapper::Release2DVideoTextures()
{
	if (m_2DTextureID && gEnv->pRenderer != nullptr)
	{
		LogDebug("<CRenderWrapper> Remove 2D texture with id <%d>", m_2DTextureID);
		gEnv->pRenderer->RemoveTexture(m_2DTextureID);
		m_2DTextureID = 0;
	}
}

int CRenderWrapper::CreateTextureForTextureVideo(int width, int height, const char* name)
{
	if (gEnv->pRenderer == nullptr)
	{
		LogError("<CRenderWrapper> Can't create texture for texture video - render nullptr");
		return 0;
	}

#if CRY_VERSION == 53 || CRY_VERSION == 54
	int textureID = gEnv->pRenderer->DownLoadToVideoMemory(nullptr, width, height, eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, name, FT_NOMIPS);
#elif CRY_VERSION == 55
	int textureID = gEnv->pRenderer->UploadToVideoMemory(nullptr, width, height, eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, name, FT_NOMIPS);
#endif
	LogDebug("<CRenderWrapper> Create texture with id <%d> name <%s> resolution <%dx%d>", textureID, name, width, height);
	return textureID;
}

void CRenderWrapper::UpdateTextureForTextureVideo(CVideoFrame * pFrame, int textureID)
{
	if (gEnv->pRenderer != nullptr)
	{
		gEnv->pRenderer->UpdateTextureInVideoMemory(textureID, pFrame->rgba(), 0, 0, pFrame->displayWidth(), pFrame->displayHeight(), eTF_R8G8B8A8);
	}
}

void CRenderWrapper::Draw2dImage(ITexture * pTex)
{
	if (pTex != nullptr && gEnv->pRenderer != nullptr)
	{
		const int textWidth = pTex->GetWidth();
		const int textHeight = pTex->GetHeight();

		const int screenWidth = gEnv->pRenderer->GetOverlayWidth();
		const int screenHeight = gEnv->pRenderer->GetOverlayHeight();

		if (textWidth > 0 && textHeight > 0 && screenWidth > 0 && screenHeight > 0)
		{
			// Draw 2D image to fullscreen window
#if CRY_VERSION == 53 || CRY_VERSION == 54
			gEnv->pRenderer->Draw2dImage(0, 0, screenWidth, screenHeight, pTex->GetTextureID(), 0.0f, 1.0f, 1.0f, 0.0f);
#elif CRY_VERSION == 55
			IRenderAuxImage::Draw2dImage(0, 0, screenWidth, screenHeight, pTex->GetTextureID(), 0.0f, 1.0f, 1.0f, 0.0f);
#endif		
		}
	}
}