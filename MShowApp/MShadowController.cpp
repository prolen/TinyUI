#include "stdafx.h"
#include "MShadowController.h"
#include "MShowApp.h"

namespace MShow
{
	MShadowController::MShadowController(MShadowView& view)
		:m_view(view),
		m_bBreak(FALSE),
		m_videoFPS(25)
	{

	}

	MShadowController::~MShadowController()
	{

	}
	BOOL MShadowController::Initialize()
	{
		TinyRectangle s;
		m_view.GetClientRect(&s);
		if (!m_dx11.Initialize(m_view.Handle(), s.Size().cx, s.Size().cy))
			return FALSE;
		return TRUE;
	}
	BOOL MShadowController::SetPulgSize(const TinySize& size)
	{
		MPreviewController* pCTRL = MShowApp::Instance().GetController().GetPreviewController();
		if (!pCTRL)
			return FALSE;
		m_pulgSize = size;
		m_image2D.Destory();
		if (!m_image2D.Load(m_dx11, pCTRL->GetRenderView().GetHandle()))
			return FALSE;
		m_copy2D.Destory();
		if (!m_copy2D.Create(m_dx11, m_pulgSize.cx, m_pulgSize.cy, NULL, TRUE))
			return FALSE;
		return TRUE;
	}
	TinySize MShadowController::GetPulgSize() const
	{
		return m_pulgSize;
	}
	void MShadowController::SetVideoFPS(INT	videoFPS)
	{
		m_videoFPS = videoFPS;
	}
	INT	MShadowController::GetVideoFPS() const
	{
		return m_videoFPS;
	}
	MShadowView& MShadowController::GetView()
	{
		return m_view;
	}
	MPacketAllocQueue&	MShadowController::GetVideoQueue()
	{
		return m_videoQueue;
	}
	void MShadowController::OnMessagePump()
	{
		TinyPerformanceTimer time;
		DWORD dwMS = static_cast<DWORD>(1000 / m_videoFPS);
		for (;;)
		{
			if (m_bBreak)
				break;
			m_copy2D.Copy(m_dx11, m_image2D);
			BYTE* bits = NULL;
			UINT pitch = 0;
			SampleTag sampleTag;
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			if (m_copy2D.Map(m_dx11, bits, pitch, TRUE))
			{
				sampleTag.size = pitch * m_pulgSize.cy;
				if (m_videoQueue.GetAllocSize() == 0)
				{
					INT count = MAX_VIDEO_QUEUE_SIZE / sampleTag.size + 1;
					m_videoQueue.Initialize(count, sampleTag.size + 4);
				}
				sampleTag.bits = static_cast<BYTE*>(m_videoQueue.Alloc());
				memcpy_s(sampleTag.bits + 4, sampleTag.size, bits, sampleTag.size);
				m_copy2D.Unmap(m_dx11);
				m_videoQueue.Push(sampleTag);
			}
			time.EndTime();
			INT delay = dwMS - static_cast<DWORD>(time.GetMillisconds());
			Sleep(delay < 0 ? 0 : delay);
		}
	}
	BOOL MShadowController::Submit()
	{
		m_bBreak = FALSE;
		return TinyTaskBase::Submit(BindCallback(&MShadowController::OnMessagePump, this));
	}
	BOOL MShadowController::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_image2D.Destory();
			m_copy2D.Destory();
			m_videoQueue.RemoveAll();
			return TRUE;
		}
		return FALSE;

	}
}