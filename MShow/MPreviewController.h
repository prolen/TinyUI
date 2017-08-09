#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "MFLVPlayer.h"
#include "Control/TinyMenu.h"
using namespace TinyUI;

namespace MShow
{
	class MPreviewView;
	/// <summary>
	/// Ԥ��
	/// </summary>
	class MPreviewController
	{
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MPreviewController)
	public:
		MPreviewController(MPreviewView& view);
		virtual ~MPreviewController();
		BOOL			Initialize();
		BOOL			Open(LPCSTR pzURL);
		BOOL			Close();
		MPreviewView&	GetView();
	private:
		void			OnVideo(BYTE* bits, LONG size);
	private:
		DX2D						m_dx2d;
		TinySize					m_videoSize;
		TinySize					m_viewSize;
		TinyComPtr<ID2D1Bitmap1>	m_bitmap1;
		MFLVPlayer					m_player;
		TinyPerformanceTimer		m_timeQPC;
		MPreviewView&				m_view;
	};
}


