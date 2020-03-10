// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "RenderWrapper.h"
#include "Frame.h"

#define VIDEO_PLAYER_2D_TEXTURE "$video_player_2D"

#include <Cry3DEngine/IIndexedMesh.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <CryRenderer/IRenderer.h>

namespace CEVPlayer
{
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

		m_2DTextureID = gEnv->pRenderer->UploadToVideoMemory(nullptr, screenWidth, screenHeight, eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, VIDEO_PLAYER_2D_TEXTURE, FT_NOMIPS);

		LogDebug("<CRenderWrapper> Create 2D texture with name <" VIDEO_PLAYER_2D_TEXTURE "> id <%d> resolution <%dx%d>", m_2DTextureID, screenWidth, screenHeight);
	}

	double CRenderWrapper::RenderFrameToMainWindow(CVideoFrame* pFrame)
	{
		if ((pFrame != nullptr && (pFrame->displayWidth() <= 0 || pFrame->displayHeight() <= 0)) || (gEnv->pRenderer == nullptr))
		{
			return -1.0;
		}
#ifdef DEBUG
		LARGE_INTEGER t1, t2, f;
		QueryPerformanceCounter(&t1);
#endif

		if (m_2DTextureID == 0)
		{
			m_2DTextureID = gEnv->pRenderer->UploadToVideoMemory(nullptr, pFrame->displayWidth(), pFrame->displayHeight(), eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, VIDEO_PLAYER_2D_TEXTURE, FT_NOMIPS);
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

		const int textureID = gEnv->pRenderer->UploadToVideoMemory(nullptr, width, height, eTF_R8G8B8A8, eTF_R8G8B8A8, 0, false, FILTER_BILINEAR, 0, name, FT_NOMIPS);

		LogDebug("<CRenderWrapper> Create texture with id <%d> name <%s> resolution <%dx%d>", textureID, name, width, height);
		return textureID;
	}

	void CRenderWrapper::UpdateTextureForTextureVideo(CVideoFrame* pFrame, int textureID)
	{
		if (gEnv->pRenderer != nullptr)
		{
			gEnv->pRenderer->UpdateTextureInVideoMemory(textureID, pFrame->rgba(), 0, 0, pFrame->displayWidth(), pFrame->displayHeight(), eTF_R8G8B8A8);
		}
	}

	void CRenderWrapper::Draw2dImage(ITexture* pTex)
	{
		if (pTex != nullptr && gEnv->pRenderer != nullptr)
		{
			const float textWidth = (float)pTex->GetWidth();
			const float textHeight = (float)pTex->GetHeight();

			const float screenWidth = (float)gEnv->pRenderer->GetWidth();
			const float screenHeight = (float)gEnv->pRenderer->GetHeight();

			if (textWidth > 0 && textHeight > 0 && screenWidth > 0 && screenHeight > 0)
			{
				IRenderAuxImage::Draw2dImage(0, 0, screenWidth, screenHeight, pTex->GetTextureID(), 0.0f, 1.0f, 1.0f, 0.0f);
			}
		}
	}
}