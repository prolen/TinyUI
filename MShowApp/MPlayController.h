#pragma once
#include "MShowCommon.h"
#include "MPlayView.h"
#include "MFLVPlayer.h"
using namespace TinyUI;

namespace MShow
{
	class MPlayView;
	/// <summary>
	/// ����
	/// </summary>
	class MPlayController : public TinyTask
	{
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MPlayController)
	public:
		MPlayController(MPlayView& view, MClock& clock);
		virtual ~MPlayController();
		BOOL	Initialize();
		BOOL	Submit();
		BOOL	Close(DWORD dwMS) OVERRIDE;
	public:
		BOOL				SetPulgSize(const TinySize& size);
		TinySize			GetPulgSize() const;
		void				SetVideoFPS(INT	videoFPS);
		INT					GetVideoFPS() const;
		MClock&				GetClock();
		MPacketAllocQueue&	GetVideoQueue();
	private:
		void	OnMessagePump();
	private:
		BOOL					m_bBreak;
		INT						m_videoFPS;
		TinySize				m_pulgSize;
		TinyPerformanceTimer	m_timer;
		MPlayView&				m_view;
		MClock&					m_clock;
		//DX11RenderView		m_renderView;
		//DX11Graphics2D		m_graphics;
		DX11Image2D				m_video2D;
		MPacketAllocQueue		m_videoQueue;
	};
}


