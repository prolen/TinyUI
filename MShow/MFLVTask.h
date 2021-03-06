#pragma once
#include "MShowCommon.h"
#include "MPacketQueue.h"
#include "MClock.h"
#include "FLVReader.h"

namespace MShow
{
	/// <summary>
	/// RTMP��ȡ�߳�
	/// </summary>
	class MFLVTask : public TinyTask
	{
		DISALLOW_COPY_AND_ASSIGN(MFLVTask)
	public:
		MFLVTask(MClock& clock, TinyMsgQueue& queue);
		virtual ~MFLVTask();
		BOOL			Initialize(LPCSTR pzURL, ErrorCallback&& callback);
		BOOL			Submit();
		BOOL			Close(DWORD dwMS) OVERRIDE;
		MPacketQueue&	GetAudioQueue();
		MPacketQueue&	GetVideoQueue();
		FLV_SCRIPTDATA&	GetScript();
		LONGLONG		GetBasePTS();
		TinySize		GetVideoSize();
	public:
		Event<void(BYTE*, LONG, WORD, BOOL&)>	EVENT_ASC;
		Event<void(BYTE*, LONG, BOOL&)>			EVENT_AVCDCR;
	private:
		void			OnMessagePump();
		BOOL			Invoke(SampleTag& tag, FLV_BLOCK& block);
	private:
		BOOL			m_bFI;
		BOOL			m_bBreak;
		LONGLONG		m_sample;
		FLVReader		m_reader;
		FLV_SCRIPTDATA	m_script;
		TinyMsgQueue&	m_msgqueue;
		MClock&			m_clock;
		MPacketQueue	m_audioQueue;
		MPacketQueue	m_videoQueue;
	};
}


