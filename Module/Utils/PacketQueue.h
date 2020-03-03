// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <queue>

namespace CEVPlayer
{
	class CPacket;

	class PacketQueue
	{
	public:
		PacketQueue() {}
		~PacketQueue() {}
	public:

		void  enqueue(CPacket* val)
		{
			m_queue.push(val);
		}

		void  pop()
		{
			m_queue.pop();
		}

		void  destroy()
		{
			while (!m_queue.empty())
				m_queue.pop();
		}

		size_t  size() const
		{
			return m_queue.size();
		}

		bool  empty() const
		{
			return size() == 0;
		}

		CPacket* first()
		{
			if (m_queue.size() > 0)
				return m_queue.front();

			return nullptr;
		}
	protected:
		std::queue<CPacket*> m_queue;
	};
}