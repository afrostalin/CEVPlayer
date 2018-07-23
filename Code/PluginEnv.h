// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include "Core/ThreadManager.h"

#include <CryRenderer/IRenderAuxGeom.h>

class C2DVideoQueue;
class CTextureVideoQueue;
class CInputDispatcher;

#define TITLE "[CEVPlayer] "
#define USE_ALIGNED_MEMORY 1
#define CRY_VERSION 55 // Setup CRYENGINE version 53, 54, 55

#define VIDEO_FOLDER "Videos/"
#define VIDEO_FORMAT_EXT ".webm"

#define SAFE_RELEASE_11(p) { if (p != nullptr) { (p)->Release(); delete p; p = nullptr; } }
#define SAFE_DELETE_11(p) { if (p != nullptr) { delete p; p = nullptr; } }
#define SAFE_DELETE_ARRAY_11(p) { if (p != nullptr) { delete[] p; p = nullptr; } }

#if USE_ALIGNED_MEMORY
#define SAFE_DELETE_ALIGNED_ARRAY(p) { if(p != nullptr) {_aligned_free(p); p = nullptr;}}
#else
#define SAFE_DELETE_ALIGNED_ARRAY(p) __pragma(message("[CRITICAL_ERROR] You try delete aligned array, but it feature disabled in project!"))
#endif

// Global plugin environment
struct SPluginEnv
{
	// Pointers
	CVideoPluginThreadManager* pThreadManager = nullptr;
	C2DVideoQueue*             pVideoQueue = nullptr;
	CTextureVideoQueue*        pTextureVideoQueue = nullptr;
	CInputDispatcher*          pInputDispatcher = nullptr;

	// Console variables
	int m_bDebugLog = 0;
	int m_bDebugDraw = 0;
	int m_bUseCrySDL = 1;
	int m_bDisableLog = 0;
};

extern SPluginEnv* mEnv;

#define Log(...) do { if(!mEnv->m_bDisableLog) { CryLog (TITLE __VA_ARGS__); } } while(0)
#define LogDebug(...) do { if(mEnv->m_bDebugLog) { CryLog (TITLE __VA_ARGS__); } } while(0)
#define LogAlways(...) do { if(!mEnv->m_bDisableLog) { CryLogAlways (TITLE __VA_ARGS__);} } while(0)
#define LogWarning(...) do { if(!mEnv->m_bDisableLog) { CryWarning(VALIDATOR_MODULE_RENDERER, VALIDATOR_WARNING, TITLE __VA_ARGS__);} } while(0)
#define LogError(...) do { if(!mEnv->m_bDisableLog) { CryWarning(VALIDATOR_MODULE_RENDERER, VALIDATOR_ERROR, TITLE __VA_ARGS__);} } while(0)

static ColorB s_debugColorWhite(255, 255, 255);
static ColorB s_debugColorYellow(255, 255, 0);
static ColorB s_debugColorRed(255, 0, 0);