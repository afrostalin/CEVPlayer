// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlugin/blob/master/LICENSE

#pragma once

#define NOMINMAX
#include <Windows.h>
#include <atomic>


class CVideoPlayerTimer
{
public:
	CVideoPlayerTimer();
	~CVideoPlayerTimer();

public:
	typedef LARGE_INTEGER TimePoint;

	TimePoint m_start;
	TimePoint m_pauseStart;

	double    m_pauseDuration;

	void      now(TimePoint& tp);
	double    secondsElapsed(const TimePoint& start, const TimePoint& end);
public:
	void      start();
	void      stop();
	void      pause();
	void      resume();

	double    elapsedSeconds();
protected:
	std::atomic<bool> m_active;
};