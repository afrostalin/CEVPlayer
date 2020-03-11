// Copyright (C) 2017-2020 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "AudioWrapper.h"

#include <CryMath/Cry_Math.h>

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <SDL_syswm.h>

namespace CEVPlayer
{
	void sdlAudioCallback(void* userPtr, Uint8* stream, int len)
	{
		CAudioWrapper* pWrapper = static_cast<CAudioWrapper*>(userPtr);

		if (pWrapper)
		{
			pWrapper->ReadAudioData((float*)stream, len / sizeof(float));
		}
	}

	CAudioWrapper::CAudioWrapper(CVideoPlayer* parent)
		: m_parent(parent)
		, m_audioDevice(0)
	{
		InitSDLAudio();
	}

	CAudioWrapper::~CAudioWrapper()
	{
		bool SDLIsInit = SDL_WasInit(SDL_INIT_AUDIO) != 0;

		if (m_audioDevice != 0)
		{
			SDL_CloseAudioDevice(m_audioDevice);
			m_audioDevice = 0;
		}

		ClearAudioData();

		if (SDLIsInit)
		{
			SDL_Quit();
		}
	}

	void CAudioWrapper::OnAudiDataDecoded(float* values, size_t count)
	{
		InsertAudioData(values, count);
	}

	void CAudioWrapper::OnVideoPlayerEvent(EVideoPlayerEvents event)
	{
		switch (event)
		{
		case EVideoPlayerEvents::OnPlay:
			SDL_PauseAudioDevice(m_audioDevice, 0);
			break;
		case EVideoPlayerEvents::OnPause:
			SDL_PauseAudioDevice(m_audioDevice, 1);
			break;
		case EVideoPlayerEvents::OnResume:
			SDL_PauseAudioDevice(m_audioDevice, 0);
			break;
		case EVideoPlayerEvents::OnSkip:
		case EVideoPlayerEvents::OnStop:
			SDL_PauseAudioDevice(m_audioDevice, 1);
			ClearAudioData();
			break;
		default:
			break;
		}
	}

	void CAudioWrapper::InsertAudioData(float* src, size_t count)
	{
		if (gEnv->pSystem->IsQuitting())
		{
			SDL_PauseAudioDevice(m_audioDevice, 1);
			ClearAudioData();
			return;
		}

		std::lock_guard<std::mutex> lock(m_audioMutex);

		m_audioBuffer.resize(m_audioBuffer.size() + count);
		memcpy(&m_audioBuffer[m_audioBuffer.size() - count], src, count * sizeof(float));
	}

	bool CAudioWrapper::ReadAudioData(float* dst, size_t count)
	{
		if (gEnv->pSystem->IsQuitting())
		{
			SDL_PauseAudioDevice(m_audioDevice, 1);
			ClearAudioData();
			return false;
		}

		std::lock_guard<std::mutex> lock(m_audioMutex);

		if (m_audioBuffer.size() < count)
			return false;

		SDL_memset(dst, 0, count * sizeof(float));

		const float volume = float(SDL_MIX_MAXVOLUME) * crymath::clamp<float>(mEnv->m_audioVolume, 0.f, 1.f);

		SDL_MixAudioFormat((UINT8*)dst, (UINT8*)&m_audioBuffer[0], AUDIO_F32, count * sizeof(float), (int)volume);

		m_audioBuffer.erase(m_audioBuffer.begin(), m_audioBuffer.begin() + count);

		return true;
	}

	bool CAudioWrapper::InitSDLAudio()
	{
		LogDebug("<CAudioWrapper> Initialization SDL audio ...");

		if (m_parent != nullptr && !m_parent->info().hasAudio)
		{
			LogWarning("<CAudioWrapper> Can't init SDL audio because parent not have any audio tracks or parent == nullptr");
			return true;
		}

		bool SDLIsInit = SDL_WasInit(SDL_INIT_AUDIO) != 0;

		SVideoInfo videoInfo = m_parent->info();

		if (!SDLIsInit)
		{
			if (SDL_Init(SDL_INIT_AUDIO) != 0)
			{
				LogError("Failed to initialize SDL (%s)", SDL_GetError());
				return false;
			}
		}

		SDL_AudioSpec wanted_spec, obtained_spec;

		wanted_spec.freq = videoInfo.audioFrequency;
		wanted_spec.format = AUDIO_F32;
		wanted_spec.channels = videoInfo.audioChannels;
		wanted_spec.silence = 0;
		wanted_spec.samples = 4096;
		wanted_spec.callback = sdlAudioCallback;
		wanted_spec.userdata = this;

		m_audioDevice = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &obtained_spec, 0);

		if (m_audioDevice == 0)
		{
			LogError("Could not create SDL audio (%s).", SDL_GetError());
			return false;
		}
		else if (wanted_spec.format != obtained_spec.format)
		{
			LogError("Wanted and obtained SDL audio formats are different! (%d : %d)", wanted_spec.format, obtained_spec.format);
			return false;
		}

		SDL_PauseAudioDevice(m_audioDevice, 0);

		LogDebug("<CAudioWrapper> Successfully initialize SDL audio");
		return true;
	}

	void CAudioWrapper::ClearAudioData()
	{
		std::lock_guard<std::mutex> lock(m_audioMutex);
		m_audioBuffer.clear();
	}
}