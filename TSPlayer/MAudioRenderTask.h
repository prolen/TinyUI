#pragma once
#include "MShowCommon.h"
#include "MAudioTask.h"
#include "MVideoTask.h"

namespace TSPlayer
{
	class MAudioTask;
	/// <summary>
	/// ������Ƶ�߳�
	/// </summary>
	class MAudioRenderTask : public TinyTask
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioRenderTask)
	public:
		MAudioRenderTask(MAudioTask& task, MClock& clock, TinyMsgQueue& queue);
		MAudioRenderTask(MAudioTask& task, MClock& clock, TinyMsgQueue& queue, TinyUI::Callback<void(BYTE*, LONG)>&& callback);
		virtual ~MAudioRenderTask();
		BOOL Initialize();
		BOOL SetVolume(DWORD dwVolume);
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL					m_bBreak;
		BOOL					m_bInitialize;
		MClock&					m_clock;
		MAudioTask&				m_task;
		TinyMsgQueue&			m_msgqueue;
		TinyXAudio				m_audio;
		TinyUI::Callback<void(BYTE*, LONG)> m_callback;
	};
}


