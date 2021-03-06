#include "stdafx.h"
#include "MVideoEncodeTask.h"
#include "MShowApp.h"

namespace MShow
{
	MVideoEncodeTask::MVideoEncodeTask(MRTMPPusher& pusher, MClock& clock)
		:m_bBreak(FALSE),
		m_pusher(pusher),
		m_videoINC(1),
		m_clock(clock)
	{
	}


	MVideoEncodeTask::~MVideoEncodeTask()
	{
	}
	BOOL MVideoEncodeTask::Submit(const TinySize& pulgSize, INT videoFPS, INT videoRate)
	{
		m_pulgSize = pulgSize;
		m_videoRate = videoRate;
		m_videoFPS = videoFPS;
		if (!m_encoder.Open(m_pulgSize, m_pulgSize, m_videoRate, m_videoFPS))
			return FALSE;
		return TinyTask::Submit(BindCallback(&MVideoEncodeTask::OnMessagePump, this));
	}

	void MVideoEncodeTask::OnMessagePump()
	{
		MPreviewController* pCTRL = MShow::MShowApp::GetInstance().GetController().GetPreviewController();
		if (pCTRL != NULL)
		{
			MPacketAllocQueue& queue = pCTRL->GetVideoQueue();
			Sample sample;
			SampleTag sampleTag;
			for (;;)
			{
				if (m_bBreak)
					break;
				if (MShow::MShowApp::GetInstance().GetController().IsPushing())
				{
					ZeroMemory(&sampleTag, sizeof(sampleTag));
					if (!queue.Pop(sampleTag))
					{
						continue;
					}
					sampleTag.sampleDTS = sampleTag.samplePTS = (m_videoINC++) * 90000u / m_videoFPS;
					ZeroMemory(&sample, sizeof(sample));
					BYTE* bo = NULL;
					LONG  so = 0;
					ZeroMemory(&sample, sizeof(sample));
					if (m_encoder.Encode(sampleTag, bo, so, sample.mediaTag))
					{
						INT sampleTime = sample.mediaTag.dwTime;
						sampleTime -= 160;
						if (sampleTime > 0)
						{
							sample.mediaTag.dwTime -= 160;
							sample.size = so;
							sample.bits = new BYTE[so];
							memcpy(sample.bits, bo, so);
							m_samples.Push(sample);
						}
					}
					queue.Free(sampleTag.bits);
				}
			}
		}
	}

	BOOL MVideoEncodeTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTask::Close(dwMS))
		{
			m_encoder.Close();
			m_samples.RemoveAll();
			return TRUE;
		}
		return FALSE;
	}
	TinySize MVideoEncodeTask::GetSize() const
	{
		return m_pulgSize;
	}
	INT MVideoEncodeTask::GetVideoFPS() const
	{
		return m_videoFPS;
	}
	INT MVideoEncodeTask::GetVideoRate() const
	{
		return m_videoRate;
	}
	QSVEncoder&	MVideoEncodeTask::GetQSV()
	{
		return m_encoder;
	}
	MSampleQueue& MVideoEncodeTask::GetSamples()
	{
		return m_samples;
	}
}