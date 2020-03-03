// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <cmath>
#include <cstdio>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <queue>

#include "Utils/SafeQueue.h"

#include "Frame.h"

namespace CEVPlayer
{
	class CVideoPlayer;

	class CVideoFrameBuffer
	{
	public:
		CVideoFrameBuffer(CVideoPlayer* parent, size_t width, size_t height, size_t frameCount);
		~CVideoFrameBuffer();
	public:
		void          reset();

		CVideoFrame* lockRead();
		void          unlockRead();

		CVideoFrame* lockWrite(double time);
		void          unlockWrite();

		void          update(double playTime, double frameTime);
		bool          isFull();
	protected:
		CVideoPlayer* m_parent;

		size_t                        m_frameCount;
		size_t                        m_width;
		size_t                        m_height;

		CVideoFrame* m_readFrame;
		std::mutex                    m_readLock;
		std::mutex                    m_updateLock;

		ThreadSafeQueue<CVideoFrame*> m_readQueue;
		ThreadSafeQueue<CVideoFrame*> m_writeQueue;
		CVideoFrame* m_writeFrame;
		double                        m_LastReadFrameTime;
	};
}