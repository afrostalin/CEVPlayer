// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "FrameBuffer.h"
#include "Video/VideoPlayer.h"

#include <cstring>


CVideoFrameBuffer::CVideoFrameBuffer(CVideoPlayer *parent, size_t width, size_t height, size_t frameCount)
	: m_parent(parent)
	, m_frameCount(frameCount)
	, m_width(width)
	, m_height(height)
	, m_writeFrame(nullptr)
	, m_LastReadFrameTime(0.0)
{
	m_readFrame = new CVideoFrame(width, height);

	for (size_t i = 0; i < frameCount; i++)
	{
		m_writeQueue.push(new CVideoFrame(width, height));
	}
}

CVideoFrameBuffer::~CVideoFrameBuffer()
{
	SAFE_DELETE_11(m_readFrame);

	while (m_writeQueue.size() > 0 && !gEnv->pSystem->IsQuitting())
	{
		CVideoFrame *frame = m_writeQueue.front();
		SAFE_DELETE_11(frame);
		m_writeQueue.pop();
	}
	
	while (m_readQueue.size() > 0 && !gEnv->pSystem->IsQuitting())
	{
		CVideoFrame *frame = m_readQueue.front();
		SAFE_DELETE_11(frame);
		m_readQueue.pop();
	}
}

CVideoFrame *CVideoFrameBuffer::lockRead()
{
	m_LastReadFrameTime = m_parent->playTime();
	double frameTime = 1.0 / m_parent->info().frameRate;

	update(m_LastReadFrameTime, frameTime);

	m_readLock.lock();

	return m_readFrame;
}

void CVideoFrameBuffer::unlockRead()
{
	m_readLock.unlock();
}

CVideoFrame *CVideoFrameBuffer::lockWrite(double time)
{
	if (m_writeQueue.empty())
	{
		return nullptr;
	}

	m_writeFrame = m_writeQueue.front();
	m_writeQueue.pop();

	m_writeFrame->setTime(time);

	return m_writeFrame;
}

void CVideoFrameBuffer::unlockWrite()
{
	m_readQueue.push(m_writeFrame);
	m_writeFrame = nullptr;
}

void CVideoFrameBuffer::reset()
{
	while (m_readQueue.size() > 0)
	{
		m_writeQueue.push(m_readQueue.front());
		m_readQueue.pop();
	}

	m_writeFrame = nullptr;
}

void CVideoFrameBuffer::update(double playTime, double frameTime)
{
	m_updateLock.lock();

	if (m_readQueue.empty())
	{
		m_updateLock.unlock();
		return;
	}

	CVideoFrame* pFrame = m_readQueue.front();

	if ((playTime - pFrame->time()) >= frameTime)
	{
		m_readLock.lock();
		pFrame->copy(m_readFrame);
		m_readLock.unlock();

		m_readQueue.pop();
		m_writeQueue.push(pFrame);
	}

	m_updateLock.unlock();
}

bool CVideoFrameBuffer::isFull()
{
	return m_writeQueue.size() == 0;
}