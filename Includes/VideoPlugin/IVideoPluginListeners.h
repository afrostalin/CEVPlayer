// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

enum class EVideoPlayerEvents
{
	OnPlay,
	OnPause,
	OnResume,
	OnStop
};

struct IVideoPlayerEventListener
{
	virtual ~IVideoPlayerEventListener() {}

	virtual void OnVideoPlayerEvent(EVideoPlayerEvents event) = 0;
};