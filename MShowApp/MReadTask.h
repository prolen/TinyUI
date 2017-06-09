#pragma once
#include "MShowCommon.h"
#include "FLVReader.h"
using namespace Decode;

namespace MShow
{
	/// <summary>
	/// RTMP����ȡ�߳�
	/// </summary>
	class MReadTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MReadTask)
	public:
		MReadTask();
		virtual ~MReadTask();
		BOOL Initialize(LPCSTR pzURL);
		BOOL Close(DWORD dwMS) OVERRIDE;
		TinyLock& GetAudioLock();
		TinyLock& GetVideoLock();
		MPacketQueue& GetAudioQueue();
		MPacketQueue& GetVideoQueue();
		H264Decode* GetH264();
		AACDecode* GetAAC();
	private:
		void OnMessagePump();
	private:
		BOOL			m_close;
		LONGLONG		m_index;
		FLVReader		m_reader;
		FLV_SCRIPTDATA	m_script;
		TinyLock		m_locks[2];
		MPacketQueue	m_audioQueue;
		MPacketQueue	m_videoQueue;
		TinyScopedPtr<H264Decode>	m_h264;
		TinyScopedPtr<AACDecode>	m_aac;
	};
}



