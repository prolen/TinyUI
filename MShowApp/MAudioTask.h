#pragma once
#include "MShowCommon.h"
#include "MClock.h"
#include "MFLVTask.h"

namespace MShow
{
	class MFLVModel;
	/// <summary>
	/// ��Ƶ�����߳�
	/// </summary>
	class MAudioTask : public TinyTask
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioTask)
	public:
		MAudioTask(MFLVTask& task, MClock& clock);
		virtual ~MAudioTask();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
		WAVEFORMATEX* GetFormat();
	public:
		MPacketAllocQueue&	GetAudioQueue();
	private:
		void OnMessagePump();
		void OnASC(BYTE* bits, LONG size, WORD wBitsPerSample, BOOL& bRes);
	private:
		BOOL						m_bBreak;
		AACDecoder					m_aac;
		MClock&						m_clock;
		MFLVTask&					m_task;
		MPacketAllocQueue			m_audioQueue;
		TinyScopedPtr<Delegate<void(BYTE*, LONG, WORD, BOOL&)>>	m_onASC;
	};
}



