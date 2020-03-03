// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <cstdio>

namespace CEVPlayer
{
	template<typename T>
	class Buffer
	{
	protected:
		T* m_data;
		size_t  m_size;

		void  resize(size_t newSize)
		{
			SAFE_DELETE_ARRAY_11(m_data);

			m_data = new T[newSize];
			m_size = newSize;
		}

	public:

		Buffer(size_t initialSize) :
			m_data(nullptr),
			m_size(0)
		{
			resize(initialSize);
		}

		~Buffer()
		{
			SAFE_DELETE_ARRAY_11(m_data);
		}

		T* get(size_t size)
		{
			if (size > m_size)
				resize(size);

			return m_data;
		}

		size_t  size() const
		{
			return m_size;
		}
	};
}