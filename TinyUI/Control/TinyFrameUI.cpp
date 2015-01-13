#include "../stdafx.h"
#include "TinyFrameUI.h"

namespace TinyUI
{
	TinyFrameUI::TinyFrameUI()
	{
	}


	TinyFrameUI::~TinyFrameUI()
	{
	}

	LRESULT TinyFrameUI::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PostQuitMessage(0);//�˳�Ӧ�ó���
		return FALSE;
	}

	BOOL TinyFrameUI::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
	{
		return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
	}
	BOOL TinyFrameUI::ShowWindow(int nCmdShow) throw()
	{
		ASSERT(::IsWindow(m_hWND));
		return ::ShowWindow(m_hWND, nCmdShow);
	}

	BOOL TinyFrameUI::UpdateWindow() throw()
	{
		ASSERT(m_hWND != NULL);
		return ::UpdateWindow(m_hWND);
	}
	DWORD TinyFrameUI::RetrieveStyle()
	{
		return (WS_VISIBLE | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS);
	}

	DWORD TinyFrameUI::RetrieveExStyle()
	{
		return (WS_EX_LEFT | WS_EX_LTRREADING);
	}

	LRESULT TinyFrameUI::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		image.Load("D:\\PNG_transparency_demonstration_1.png");
		//image1.Load("D:\\bianping.png");
		
		return TRUE;
	}

	LPCSTR TinyFrameUI::RetrieveClassName()
	{
		return TEXT("FramwUI");
	}

	LPCSTR TinyFrameUI::RetrieveTitle()
	{
		return TEXT("FramwUI");
	}

	HICON TinyFrameUI::RetrieveIcon()
	{
		return NULL;
	}

	LRESULT TinyFrameUI::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		PAINTSTRUCT s;
		HDC hDC = BeginPaint(m_hWND, &s);
		TinySize size = image.GetSize();
		TinySize size1 = image1.GetSize();
		TinySize paintSize((s.rcPaint.right - s.rcPaint.left), (s.rcPaint.bottom - s.rcPaint.top));
		TinyRectangle paintRect(0, 0, m_cx, m_cy);
		//����

		/*HDC hMemDC = CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, image);

		HDC hMenDC1 = CreateCompatibleDC(hMemDC);
		HBITMAP hOldBitmap1 = (HBITMAP)SelectObject(hMenDC1, image1);
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		::AlphaBlend(hMemDC, 0, 0, size1.cx, size1.cy, hMenDC1, 0, 0, size1.cx, size1.cy, blend);
		SelectObject(hMenDC1, hOldBitmap1);
		DeleteDC(hMenDC1);

		::BitBlt(hDC, 0, 0, size.cx, size.cy, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);*/

		HDC hMemDC = CreateCompatibleDC(hDC);
		HBITMAP hBitmap = CreateCompatibleBitmap(hDC, m_cx, m_cy);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
		::FillRect(hMemDC, &paintRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

		HDC hMenDC1 = CreateCompatibleDC(hMemDC);
		HBITMAP hOldBitmap1 = (HBITMAP)SelectObject(hMenDC1, image);
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
		::AlphaBlend(hMemDC, 0, 0, size.cx, size.cy, hMenDC1, 0, 0, size.cx, size.cy, blend);
		SelectObject(hMenDC1, hOldBitmap1);
		DeleteDC(hMenDC1);

		::BitBlt(hDC, 0, 0, m_cx, m_cy, hMemDC, 0, 0, SRCCOPY);
		SelectObject(hMemDC, hOldBitmap);
		DeleteObject(hBitmap);
		DeleteDC(hMemDC);

		//HDC hMenDC = CreateCompatibleDC(hDC);
		//HBITMAP hBitmap = CreateCompatibleBitmap(hDC, size.cx, size.cy);
		//HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMenDC, hBitmap);

		////::FillRect(hMenDC, &s.rcPaint, (HBRUSH)GetStockObject(GRAY_BRUSH));

		//SelectObject(hMenDC, image);
		//::BitBlt(hDC, 0, 0, size.cx, size.cy, hMenDC, 0, 0, SRCCOPY);

		//DeleteObject(hBitmap);
		//SelectObject(hMenDC, hOldBitmap);
		//DeleteDC(hMenDC);

		///*HDC hMenDC1 = CreateCompatibleDC(hDC);
		//HBITMAP hBitmap1 = (HBITMAP)SelectObject(hMenDC1, image);

		//::BitBlt(hMenDC, 0, 0, size.cx, size.cy, hMenDC1, 0, 0, SRCCOPY);
		//::BitBlt(hDC, 0, 0, size.cx, size.cy, hMenDC, 0, 0, SRCCOPY);

		//DeleteObject(hBitmap);
		//DeleteDC(hMenDC1);
		//DeleteDC(hMenDC);*/

		//EndPaint(m_hWND, &s);
		///*HDC hMenDC = CreateCompatibleDC(hDC);
		//HBITMAP hBitmap = CreateCompatibleBitmap(hDC, m_cx, m_cy);
		//HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMenDC, hBitmap);
		//RECT rc = { 0, 0, m_cx, m_cy };
		//::FillRect(hMenDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

		//TRACE("OnPaint: %d,%d\n", paintSize.cx, paintSize.cy);

		//BLENDFUNCTION bs = { AC_SRC_OVER, 0, 128, 1 };
		//::AlphaBlend(hDC, 10, 10, size.cx, size.cy, hMenDC, 0, 0, size.cx, size.cy, bs);

		//SelectObject(hMenDC, hOldBitmap);
		//DeleteDC(hMenDC);*/



		return FALSE;
	}

	LRESULT TinyFrameUI::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return TRUE;
	}

	LRESULT TinyFrameUI::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_cx = LOWORD(lParam);
		m_cy = HIWORD(lParam);
		return FALSE;
	}

}

