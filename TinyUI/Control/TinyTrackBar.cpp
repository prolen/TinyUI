#include "../stdafx.h"
#include "TinyTrackBar.h"

namespace TinyUI
{
	IMPLEMENT_DYNAMIC(TinyTrackBar, TinyControl);
	TinyTrackBar::TinyTrackBar()
		:m_currentPos(0)
	{

	}
	TinyTrackBar::~TinyTrackBar()
	{

	}
	LPCSTR TinyTrackBar::RetrieveClassName()
	{
		return TRACKBAR_CLASS;
	}
	LPCSTR TinyTrackBar::RetrieveTitle()
	{
		return TEXT("TinyTrackBar");
	}
	DWORD TinyTrackBar::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_CHILDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_EX_LEFT | TBS_HORZ | TBS_NOTICKS | TBS_TOOLTIPS);
	}
	DWORD TinyTrackBar::RetrieveExStyle()
	{
		return WS_EX_LTRREADING;
	}
	BOOL TinyTrackBar::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		INITCOMMONCONTROLSEX initCtrls = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
		if (InitCommonControlsEx(&initCtrls))
		{
			return TinyControl::Create(hParent, x, y, cx, cy);
		}
		return FALSE;
	}
	LRESULT TinyTrackBar::OnNotifyReflect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		switch (LOWORD(wParam))
		{
		case TB_LINEUP:
		case TB_LINEDOWN:
		case TB_PAGEUP:
		case TB_PAGEDOWN:
		case TB_THUMBTRACK:
		case TB_TOP:
		case TB_BOTTOM:
		case TB_ENDTRACK:
		{
			INT newPos = GetPos();
			if (m_currentPos != newPos)
			{
				EVENT_POSCHANGING(this, newPos);
				m_currentPos = newPos;
			}
		}
		break;
		}
		return FALSE;
	}
	INT TinyTrackBar::GetLineSize() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_GETLINESIZE, 0, 0l);
	}
	INT TinyTrackBar::SetLineSize(INT nSize)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_SETLINESIZE, 0, nSize);
	}
	INT TinyTrackBar::GetPageSize() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_GETPAGESIZE, 0, 0l);
	}
	INT TinyTrackBar::SetPageSize(INT nSize)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_SETPAGESIZE, 0, nSize);
	}
	void TinyTrackBar::GetRange(INT& nMin, INT& nMax) const
	{
		ASSERT(::IsWindow(m_hWND));
		nMin = GetRangeMin();
		nMax = GetRangeMax();
	}
	INT TinyTrackBar::GetRangeMax() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_GETRANGEMAX, 0, 0l);
	}
	INT TinyTrackBar::GetRangeMin() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_GETRANGEMIN, 0, 0l);
	}
	void TinyTrackBar::SetRange(INT nMin, INT nMax, BOOL bRedraw)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_SETRANGE, bRedraw, MAKELONG(nMin, nMax));
	}
	void TinyTrackBar::SetRangeMin(INT nMin, BOOL bRedraw)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_SETRANGEMIN, bRedraw, nMin);
	}
	void TinyTrackBar::SetRangeMax(INT nMax, BOOL bRedraw)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_SETRANGEMAX, bRedraw, nMax);
	}
	void TinyTrackBar::GetSelection(INT& nMin, INT& nMax) const
	{
		ASSERT(::IsWindow(m_hWND));
		nMin = INT(::SendMessage(m_hWND, TBM_GETSELSTART, 0, 0L));
		nMax = INT(::SendMessage(m_hWND, TBM_GETSELEND, 0, 0L));
	}

	void TinyTrackBar::SetSelection(INT nMin, INT nMax)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_SETSELSTART, 0, (LPARAM)nMin);
		::SendMessage(m_hWND, TBM_SETSELEND, 0, (LPARAM)nMax);
	}
	void TinyTrackBar::ClearSel(BOOL bRedraw)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_CLEARSEL, bRedraw, 0l);
	}
	void TinyTrackBar::GetChannelRect(LPRECT lprc) const
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_GETCHANNELRECT, 0, (LPARAM)lprc);
	}
	void TinyTrackBar::GetThumbRect(LPRECT lprc) const
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_GETTHUMBRECT, 0, (LPARAM)lprc);
	}
	INT TinyTrackBar::GetPos() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_GETPOS, 0, 0l);
	}
	void TinyTrackBar::SetPos(INT nPos)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_SETPOS, TRUE, nPos);
	}
	void TinyTrackBar::ClearTics(BOOL bRedraw)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_CLEARTICS, bRedraw, 0l);
	}
	UINT TinyTrackBar::GetNumTics() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (UINT) ::SendMessage(m_hWND, TBM_GETNUMTICS, 0, 0l);
	}
	DWORD* TinyTrackBar::GetTicArray() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (DWORD*) ::SendMessage(m_hWND, TBM_GETPTICS, 0, 0l);
	}
	INT TinyTrackBar::GetTic(INT nTic) const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_GETTIC, nTic, 0L);
	}
	INT TinyTrackBar::GetTicPos(INT nTic) const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_GETTICPOS, nTic, 0L);
	}
	BOOL TinyTrackBar::SetTic(INT nTic)
	{
		ASSERT(::IsWindow(m_hWND));
		return (BOOL)::SendMessage(m_hWND, TBM_SETTIC, 0, nTic);
	}
	void TinyTrackBar::SetTicFreq(INT nFreq)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_SETTICFREQ, nFreq, 0L);
	}
	HWND TinyTrackBar::GetBuddy(BOOL fLocation) const
	{
		ASSERT(::IsWindow(m_hWND));
		return  (HWND)::SendMessage(m_hWND, TBM_GETBUDDY, fLocation, 0l);
	}
	HWND TinyTrackBar::SetBuddy(HWND hWndBuddy, BOOL fLocation)
	{
		ASSERT(::IsWindow(m_hWND));
		return (HWND)::SendMessage(m_hWND, TBM_SETBUDDY, fLocation, (LPARAM)hWndBuddy);
	}
	HWND TinyTrackBar::GetToolTips() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (HWND)::SendMessage(m_hWND, TBM_GETTOOLTIPS, 0, 0L);
	}
	void TinyTrackBar::SetToolTips(HWND hTip)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_SETTOOLTIPS, (WPARAM)hTip, 0L);
	}
	INT TinyTrackBar::SetTipSide(INT nLocation)
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT)::SendMessage(m_hWND, TBM_SETTIPSIDE, nLocation, 0L);
	}
	void TinyTrackBar::SetThumbLength(INT nLength)
	{
		ASSERT(::IsWindow(m_hWND));
		::SendMessage(m_hWND, TBM_SETTHUMBLENGTH, (WPARAM)nLength, 0L);
	}
	INT TinyTrackBar::GetThumbLength() const
	{
		ASSERT(::IsWindow(m_hWND));
		return (INT) ::SendMessage(m_hWND, TBM_GETTHUMBLENGTH, 0, 0L);
	}
}

