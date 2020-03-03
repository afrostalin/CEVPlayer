// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <queue>
#include <mutex>

namespace CEVPlayer
{
	template<typename T>
	class ThreadSafeQueue
	{
	protected:
		std::queue<T>  m_queue;
		mutable std::mutex  m_mutex;

	public:

		ThreadSafeQueue() {}
		~ThreadSafeQueue() {}

		void  push(T val)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push(val);
		}

		void  pop()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.pop();
		}

		T  front() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_queue.front();
		}

		size_t  size() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_queue.size();
		}

		void  destroy()
		{
			while (!m_queue.empty())
				m_queue.pop();
		}

		bool  empty() const
		{
			return size() == 0;
		}

		T     first()
		{
			if (m_queue.size() > 0)
				return m_queue.front();

			return nullptr;
		}
	};
}