// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "Packet.h"


CPacket::CPacket(const mkvparser::Block *block, CPacket::Type type, double time)
	: m_block(block)
	, m_type(type)
	, m_time(time)
{
}

CPacket::~CPacket()
{
}

CPacket::Type  CPacket::type() const
{
	return m_type;
}

const mkvparser::Block *CPacket::block() const
{
	return m_block;
}

double  CPacket::time() const
{
	return m_time;
}