// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlayer/blob/master/LICENSE

#pragma once

#include "Core/ThreadManager.h"

#include <CryRenderer/IRenderAuxGeom.h>

class C2DVideoQueue;
class CTextureVideoQueue;
class CInputDispatcher;

#define TITLE "[CryVideoPlayer] "
#define USE_ALIGNED_MEMORY 1
#define CRYENGINE_5_3 (0) //! Change it to 1 if you need build plugin for CRYENGINE 5.3

#define VIDEOS_FOLDER "Videos/"
#define VEDEO_FORMAT_EXT ".webm"

#define SAFE_DELETE_11(p) { if (p != nullptr) { delete p; p = nullptr; } }
#define SAFE_DELETE_ARRAY_11(p) { if (p != nullptr) { delete[] p; p = nullptr; } }

#if USE_ALIGNED_MEMORY
#define SAFE_DELETE_ALIGNED_ARRAY(p) { if(p != nullptr) {_aligned_free(p); p = nullptr;}}
#else
#define SAFE_DELETE_ALIGNED_ARRAY(p) __pragma(message("[CRITICAL_ERROR] You try delete aligned array, but it feature disabled in project!"))
#endif

#define Log(...) do {  CryLog (TITLE __VA_ARGS__); } while(0)
#define LogAlways(...) do { CryLogAlways (TITLE __VA_ARGS__); } while(0)
#define LogWarning(...) do { CryWarning(VALIDATOR_MODULE_RENDERER, VALIDATOR_WARNING, TITLE __VA_ARGS__); } while(0)
#define LogError(...) do { CryWarning(VALIDATOR_MODULE_RENDERER, VALIDATOR_ERROR, TITLE __VA_ARGS__); } while(0)

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
};

extern SPluginEnv* mEnv;

#define LogDebug(...) do {  if(mEnv->m_bDebugLog) {CryLog (TITLE __VA_ARGS__);} } while(0)

static ColorB s_debugColorWhite(255, 255, 255);
static ColorB s_debugColorYellow(255, 255, 0);
static ColorB s_debugColorRed(255, 0, 0);