#pragma once
#include "MShowCommon.h"
#include "MShadowView.h"
#include "MPacketQueue.h"
#include "MClock.h"
using namespace TinyUI;

namespace MShow
{
	class MShadowView;
	/// <summary>
	/// Ӱ��UI������
	/// </summary>
	class MShadowController : public TinyTask
	{
		friend class MShowController;
		friend class MVideoController;
		friend class MImageController;
		friend class MRTMPEncoder;
		DISALLOW_COPY_AND_ASSIGN(MShadowController)
	public:
		MShadowController(MShadowView& view, MClock& clock);
		virtual ~MShadowController();
		BOOL		Initialize();
		BOOL		SetPulgSize(const TinySize& size);
		TinySize	GetPulgSize() const;
		void		SetVideoFPS(INT	videoFPS);
		INT			GetVideoFPS() const;
		MClock&		GetClock();
		BOOL		Start();
		void		Stop();
		BOOL		Uninitialize();
	public:
		BOOL		Submit();
		BOOL		Close(DWORD dwMS) OVERRIDE;
	public:
		MShadowView&		GetView();
		MPacketAllocQueue&	GetVideoQueue();
	private:
		DWORD		OnVideo(SampleTag& sampleTag);
		void		OnTimer();
		void		OnMessagePump();
	private:
		BOOL					m_bBreak;
		INT						m_videoFPS;
		TinySize				m_pulgSize;
		MClock&					m_clock;
		MShadowView&			m_view;
		MPacketQueue			m_queue;
		MPacketAllocQueue		m_videoQueue;
		TinyPerformanceTimer	m_timer;
		TinyPerformanceTime		m_timeQPC;
		DX11					m_dx11;
		DX11Texture2D			m_image2D;
		DX11Texture2D			m_copy2D;
	};
}


