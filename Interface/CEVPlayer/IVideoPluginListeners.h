// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

namespace CEVPlayer
{
	enum class EVideoPlayerEvents
	{
		OnPlay,
		OnPause,
		OnResume,
		OnStop,
		OnSkip, // This event executed when user try skip video & used for UI skiping
	};

	struct IVideoPlayerEventListener
	{
		virtual ~IVideoPlayerEventListener() {}

		virtual void OnVideoPlayerEvent(const char* videoFileName, CEVPlayer::EVideoPlayerEvents event) = 0;
	};
}