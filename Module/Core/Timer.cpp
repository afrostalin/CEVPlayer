// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "Timer.h"

#include <ctime>
#include <chrono>

LARGE_INTEGER s_timerFrequency;

CVideoPlayerTimer::CVideoPlayerTimer()
	: m_active(false)
	, m_pauseDuration(0.0)
{
	QueryPerformanceFrequency(&s_timerFrequency);
}

CVideoPlayerTimer::~CVideoPlayerTimer()
{
}

void  CVideoPlayerTimer::start()
{
	now(m_start);

	m_active = true;
	m_pauseDuration = 0.0;
}

void  CVideoPlayerTimer::stop()
{
	m_active = false;
}

void  CVideoPlayerTimer::pause()
{
	now(m_pauseStart);
}

void  CVideoPlayerTimer::resume()
{
	TimePoint end;
	now(end);

	m_pauseDuration += secondsElapsed(m_pauseStart, end);
}

double CVideoPlayerTimer::elapsedSeconds()
{
	if (!m_active)
		return 0.0;

	TimePoint end;
	now(end);

	return secondsElapsed(m_start, end) - m_pauseDuration;
}

void CVideoPlayerTimer::now(TimePoint & tp)
{
	QueryPerformanceCounter(&tp);
}

double CVideoPlayerTimer::secondsElapsed(const TimePoint& start, const TimePoint& end)
{
	return (end.QuadPart - start.QuadPart) / (double)s_timerFrequency.QuadPart;
}
