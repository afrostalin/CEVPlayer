// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlugin/blob/master/LICENSE

#pragma once

#include <mkvparser/mkvparser.hpp>

class CPacket
{
public:
	enum class Type
	{
		Video,
		Audio
	};

	CPacket(const mkvparser::Block *block, Type type, double time);
	~CPacket();
public:
	Type                    type() const;
	const mkvparser::Block* block() const;
	double                  time() const;
protected:
	const mkvparser::Block* m_block;
	Type                    m_type;
	double                  m_time;
};