#pragma once
#include "Common.h"
#include "AACDecoder.h"
#include "MPG123Decode.h"
#include "x264Decoder.h"
#include <vector>

namespace Decode
{
#define SAVC(x)    static const AVal av_##x = AVC(#x)
	/// <summary>
	/// FLV目前只支持AAC+H264
	/// </summary>
	class FLVWriter
	{
		DISALLOW_COPY_AND_ASSIGN(FLVWriter)
	public:
		FLVWriter();
		virtual ~FLVWriter();
		BOOL Create(LPCSTR pzFile);
		BOOL Close();
		BOOL WriteScriptTag(FLV_SCRIPTDATA& script);
		BOOL WriteAACASC(BYTE* bits, LONG size);
		BOOL WriteAACRaw(BYTE* bits, LONG size, LONGLONG timestamp);
		BOOL WriteH264AVC(const vector<BYTE>& sps, const vector<BYTE>& pps);
		BOOL WriteH264NALU(BYTE frameType, BYTE* bits, LONG size, LONGLONG pts, LONGLONG dts);
	private:
		DWORD					m_dwPreviousSize;
		TinyComPtr<IStream>		m_stream;
		BOOL WriteAudioTag(FLV_PACKET& packet, BYTE* bits, LONG size);
		BOOL WriteVideoTag(FLV_PACKET& packet, BYTE* bits, LONG size);
	};
}

