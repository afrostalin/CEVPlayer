// Copyright (C) 2017-2020 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <vector>
#include <mutex>

#include "Video/VideoPlayer.h"

#include <SDL.h>
#include <SDL_audio.h>

namespace CEVPlayer
{
	class CAudioWrapper
	{
	public:
		explicit CAudioWrapper(CVideoPlayer* parent);
		~CAudioWrapper();
	public:
		void OnAudiDataDecoded(float* values, size_t count);
		void OnVideoPlayerEvent(EVideoPlayerEvents event);
	public:
		void InsertAudioData(float* src, size_t count);
		bool ReadAudioData(float* dst, size_t count);
	private:
		bool InitSDLAudio();
		void ClearAudioData();
	private:
		CVideoPlayer* m_parent;
		SDL_AudioDeviceID  m_audioDevice;
		std::vector<float> m_audioBuffer;
		std::mutex         m_audioMutex;
	};
}