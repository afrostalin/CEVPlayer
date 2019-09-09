// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "Frame.h"

#include <vpx/vpx_image.h>


CVideoFrame::CVideoFrame(size_t width, size_t height)
	: m_displayWidth(width)
	, m_displayHeight(height)
	, m_time(0.0)
	, m_isValid(false)
{
	m_nSize = width * height * 4;

#if USE_ALIGNED_MEMORY
	m_rgba = (unsigned char*)_aligned_malloc(m_nSize, 16);
#else
	m_rgba = new unsigned char[m_nSize];
#endif
}

CVideoFrame::~CVideoFrame()
{
#if USE_ALIGNED_MEMORY
	SAFE_DELETE_ALIGNED_ARRAY(m_rgba);
#else
	SAFE_DELETE_ARRAY_11(m_rgba);
#endif
}

unsigned char * CVideoFrame::rgba() const
{
	return m_rgba;
}

size_t CVideoFrame::displayWidth() const
{
	return m_displayWidth;
}

size_t CVideoFrame::displayHeight() const
{
	return m_displayHeight;
}

void  CVideoFrame::setTime(double time)
{
	m_time = time;
}

double  CVideoFrame::time() const
{
	return m_time;
}

bool CVideoFrame::isValid() const
{
	return m_isValid;
}

void CVideoFrame::copy(CVideoFrame *dst)
{
	std::memcpy(dst->rgba(), m_rgba, m_nSize);

	dst->setTime(m_time);
	dst->setValid(true);
}