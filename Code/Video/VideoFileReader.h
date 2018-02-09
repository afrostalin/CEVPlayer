// Copyright (C) 2017-2018 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CryVideoPlayer/blob/master/LICENSE

#pragma once

#include <mkvparser/mkvreader.hpp>

class CVideoFileReader : public mkvparser::IMkvReader
{
public:
	CVideoFileReader();
	virtual ~CVideoFileReader();
public:
	int  Open(const char *fileName, bool preloadFile = false);
	void Close();

	// IMkvReader
	int    Read(long long position, long length, unsigned char* buffer) override;
	int    Length(long long* total, long long* available) override;
	// ~IMkvReader
public:
	std::string    GetFileName() { return m_fileName; }
private:
	bool           GetVideoFileSize();
private:
	long long      m_length;
	FILE*          m_file;
	std::string    m_fileName;
private:
	bool           m_bPreloaded;
	unsigned char* m_PreloadedData;
};