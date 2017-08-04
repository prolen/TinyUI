#include "stdafx.h"
#include "MVideoEncodeTask.h"
#include "MShowApp.h"

namespace MShow
{
	MVideoEncodeTask::MVideoEncodeTask(MRTMPPusher& pusher, MClock& clock)
		:m_bBreak(FALSE),
		m_videoSize(0),
		m_pusher(pusher),
		m_videoINC(0),
		m_clock(clock)
	{
	}


	MVideoEncodeTask::~MVideoEncodeTask()
	{
	}

	BOOL MVideoEncodeTask::Initialize(MPreviewView& view)
	{
		TinyRectangle s;
		view.GetClientRect(&s);
		if (!m_dx11.Initialize(view.Handle(), s.Size().cx, s.Size().cy))
			return FALSE;
		return TRUE;
	}
	BOOL MVideoEncodeTask::Submit(const TinySize& pulgSize, INT videoFPS, INT videoRate)
	{
		MPreviewController* pCTRL = MShowApp::Instance().GetController().GetPreviewController();
		if (!pCTRL)
			return FALSE;
		m_pulgSize = pulgSize;
		m_videoRate = videoRate;
		m_videoFPS = videoFPS;
		if (!m_image2D.Load(m_dx11, pCTRL->GetRenderView().GetHandle()))
			return FALSE;
		if (!m_copy2D.Create(m_dx11, m_pulgSize.cx, m_pulgSize.cy, NULL, TRUE))
			return FALSE;
		if (!m_encoder.Open(m_pulgSize, m_pulgSize, m_videoRate, m_videoFPS))
			return FALSE;
		return TinyTaskBase::Submit(BindCallback(&MVideoEncodeTask::OnMessagePump, this));
	}
	void MVideoEncodeTask::OnMessagePump()
	{
		BOOL bFirst = FALSE;
		TinyPerformanceTimer time;
		DWORD dwMS = static_cast<DWORD>(1000 / m_videoFPS);
		for (;;)
		{
			if (m_bBreak)
				break;
			time.BeginTime();
			m_copy2D.Copy(m_dx11, m_image2D);
			BYTE* bits = NULL;
			UINT pitch = 0;
			if (m_copy2D.Map(m_dx11, bits, pitch, TRUE))
			{
				DWORD dwSize = pitch * m_pulgSize.cy;
				if (m_videoSize != dwSize && bits != NULL)
				{
					m_videoSize = dwSize;
					m_videoBits.Reset(new BYTE[m_videoSize]);
				}
				memcpy_s(m_videoBits, m_videoSize, bits, m_videoSize);
				m_copy2D.Unmap(m_dx11);
			}
			if (m_videoBits != NULL)
			{
				m_timer.BeginTime();
				SampleTag sampleTag = { 0 };
				sampleTag.bits = m_videoBits;
				sampleTag.size = m_videoSize;
				sampleTag.sampleDTS = sampleTag.samplePTS = (m_videoINC++) * 90000 / m_videoFPS;
				BYTE* bo = NULL;
				LONG  so = 0;
				Sample sample;
				ZeroMemory(&sample, sizeof(sample));
				if (m_encoder.Encode(sampleTag, bo, so, sample.mediaTag))
				{
					m_timer.EndTime();
					TRACE("Cost Time:%d\n", m_timer.GetMillisconds());
					if (m_clock.GetBaseTime() == -1)
					{
						m_clock.SetBaseTime(timeGetTime());
					}
					if (m_clock.GetBaseTime() != -1)
					{
						//TRACE("Video Time:%d\n", sample.mediaTag.dwTime);
						sample.size = so;
						sample.bits = new BYTE[so];
						memcpy(sample.bits, bo, so);
						m_samples.Push(sample);
					}

				}
			}
			time.EndTime();
			INT delay = dwMS - static_cast<DWORD>(time.GetMillisconds());
			Sleep(delay < 0 ? 0 : delay);
		}
	}
	BOOL MVideoEncodeTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_image2D.Destory();
			m_copy2D.Destory();
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