// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <cstdio>
#include <memory>

namespace CEVPlayer
{
	class CVideoFrame
	{
	public:
		CVideoFrame(size_t width, size_t height);
		~CVideoFrame();
	public:
		void           copy(CVideoFrame* dst);

		unsigned char* rgba() const;

		size_t         displayWidth() const;
		size_t         displayHeight() const;

		void           setTime(double time);
		double         time() const;

		void           setValid(bool valid) { m_isValid = valid; }
		bool           isValid() const;
	private:
		unsigned char* m_rgba;
		unsigned int   m_nSize;
		size_t         m_displayWidth;
		size_t         m_displayHeight;
		double         m_time;
		bool           m_isValid;
	};
}