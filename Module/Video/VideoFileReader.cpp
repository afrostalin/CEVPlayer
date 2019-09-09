// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#include "StdAfx.h"
#include "PluginEnv.h"
#include "VideoFileReader.h"

CVideoFileReader::CVideoFileReader() 
	: m_length(0)
	, m_file(nullptr)
	, m_bPreloaded(false)
	, m_PreloadedData(nullptr)
{
}

CVideoFileReader::~CVideoFileReader()
{
	Close();	
}

int CVideoFileReader::Open(const char *fileName, bool preloadFile)
{
	LogDebug("<CVideoFileReader> Opening video file <%s> preload <%s>", fileName, preloadFile ? "true" : "false");

	if (fileName == nullptr)
	{
		LogWarning("<CVideoFileReader> Can't open file - filename empty");
		return -1;
	}

	if (m_file != nullptr)
	{
		LogWarning("<CVideoFileReader> Can't open file - reader alredy open file, close it before open new file!");
		return -1;
	}

	m_file = gEnv->pCryPak->FOpen(fileName, "rb");

	if (m_file == nullptr)
	{
		LogWarning("<CVideoFileReader> Can't open file - file not found!");
		return -1;
	}

	m_fileName = fileName;

	bool result = !GetVideoFileSize();

	if (preloadFile)
	{
		m_PreloadedData = new unsigned char[m_length];
		Read((long long)0L, (long)m_length, m_PreloadedData);
		gEnv->pCryPak->FClose(m_file);
		m_bPreloaded = preloadFile;
	}

	return result;
}

void CVideoFileReader::Close()
{
	if (m_bPreloaded)
	{
		LogDebug("<CVideoFileReader> Clear preload data...", m_fileName.c_str());
		SAFE_DELETE_ARRAY_11(m_PreloadedData);
	}
	else if (m_file != nullptr)
	{
		LogDebug("<CVideoFileReader> Closing video file <%s>", m_fileName.c_str());
		gEnv->pCryPak->FClose(m_file);
	}

	m_file = nullptr;
}

int CVideoFileReader::Read(long long offset, long len, unsigned char* buffer)
{
	if (m_file == nullptr)
	{
		LogWarning("<CVideoFileReader> Can't read data - video file not open!");
		return -1;
	}

	if (offset < 0)
	{
		LogWarning("<CVideoFileReader> Can't read data - offset can't be < 0!");
		return -1;
	}

	if (len <= 0)
	{
		LogWarning("<CVideoFileReader> Can't read data - len for read can't be <= 0!");
		return -1;
	}

	if (offset > m_length)
	{
		LogWarning("<CVideoFileReader> Can't read data - offset can't be > file lenght!");
		return -1;
	}

	if (!m_bPreloaded)
	{
		const size_t status = gEnv->pCryPak->FSeek(m_file, (long)offset, SEEK_SET);
		if (status)
		{
			LogWarning("<CVideoFileReader> Can't read data - can't file seek!");
			return -1;
		}

		const size_t size = gEnv->pCryPak->FReadRaw(buffer, 1, len, m_file);

		if (size < size_t(len))
		{
			LogWarning("<CVideoFileReader> Can't read data - can't read from file!");
			return -1;
		}
	}
	else
	{
		size_t size = len;

		if ((offset + len) > m_length)
			size = m_length - offset;

		memcpy(buffer, m_PreloadedData + offset, size);

		if (size < size_t(len))
		{
			LogWarning("<CVideoFileReader> Can't read data - can't read from preloaded buffer!");
			return -1;
		}
	}

	return 0;
}

int CVideoFileReader::Length(long long* total, long long* available)
{
	if (m_file == nullptr)
	{
		LogWarning("<CVideoFileReader> Can't get length - video file not open!");
		return -1;
	}

	if (total)
		*total = m_length;

	if (available)
		*available = m_length;

	return 0;
}

bool  CVideoFileReader::GetVideoFileSize()
{
	if (m_file == nullptr)
		return false;

	size_t status = gEnv->pCryPak->FSeek(m_file, 0L, SEEK_END);

	if (status)
		return false;  // error

	m_length = gEnv->pCryPak->FTell(m_file);

	if (m_length < 0)
		return false;

	status = gEnv->pCryPak->FSeek(m_file, 0L, SEEK_SET);

	if (status)
		return false;  // error


	return true;
}