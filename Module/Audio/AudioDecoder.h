// Copyright (C) 2017-2019 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <vorbis/codec.h>

#include "Utils/Buffer.h"

namespace CEVPlayer
{
	class CAudioData
	{
	protected:
		float* mValues;
		size_t  mSize;

	public:
		CAudioData(float* values, size_t size)
			: mValues(values)
			, mSize(size)
		{
		}

		~CAudioData()
		{
			delete[] mValues;
		}

		const float* values() { return mValues; }
		size_t  size() { return mSize; }
	};

	class CVideoPlayer;

	class CAudioDecoder
	{
	public:
		CAudioDecoder(CVideoPlayer* parent, size_t bufferSize);
		~CAudioDecoder();
	public:
		void                init();
		bool                initHeader(unsigned char* data, size_t size);
		bool                postInit();

		void                resetDecode();
		bool                decodeHeader(const unsigned char* data, size_t length);
		int                 decode(const unsigned char* aData, size_t aLength,
			int64_t aOffset, uint64_t aTstampUsecs, uint64_t aTstampDuration,
			int64_t aDiscardPadding, int32_t* aTotalFrames);
		long                rate();
		int                 channels();

		float               decodedTime();
		size_t              bufferSize() const;
	protected:
		vorbis_info         m_vorbisInfo;
		vorbis_comment      m_vorbisComment;
		vorbis_dsp_state    m_vorbisDsp;
		vorbis_block        m_vorbisBlock;
		int64_t             m_packetCount;
		CVideoPlayer* m_parent;
		uint64_t            m_framesDecoded;
		Buffer<float>* m_buffer;
	};
}