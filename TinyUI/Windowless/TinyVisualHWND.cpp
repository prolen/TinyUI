#include "../stdafx.h"
#include "TinyVisualHWND.h"
#include "../Render/TinyTransform.h"
#include "../Render/TinyCanvas.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualHWND::TinyVisualHWND()
			:m_vtree(NULL)
		{

		}
		TinyVisualHWND::~TinyVisualHWND()
		{

		}
		BOOL TinyVisualHWND::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
		{
			return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
		}
		DWORD TinyVisualHWND::RetrieveStyle()
		{
			return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW);
		}
		DWORD TinyVisualHWND::RetrieveExStyle()
		{
			return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
		}
		LPCSTR TinyVisualHWND::RetrieveClassName()
		{
			return TEXT("TinyVisualHWND");
		}
		LPCSTR TinyVisualHWND::RetrieveTitle()
		{
			return TEXT("FramwUI");
		}
		HICON TinyVisualHWND::RetrieveIcon()
		{
			return NULL;
		}
		void TinyVisualHWND::Initialize()
		{
			m_vtree = new TinyVisualTree(this);
			m_vtree->Initialize();
		}
		void TinyVisualHWND::Uninitialize()
		{
			if (!m_vtree) return;
			m_vtree->Uninitialize();
			SAFE_DELETE(m_vtree);
		}
		LRESULT TinyVisualHWND::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			PAINTSTRUCT ps = { 0 };
			HDC hDC = BeginPaint(m_hWND, &ps);
			EndPaint(m_hWND, &ps);
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			m_size.cx = LOWORD(lParam);
			m_size.cy = HIWORD(lParam);
			return FALSE;
		}

		LRESULT TinyVisualHWND::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			Uninitialize();
			PostQuitMessage(0);//�˳�Ӧ�ó���
			return FALSE;
		}

		LRESULT TinyVisualHWND::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			Initialize();
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;

			return FALSE;
		}
		LRESULT TinyVisualHWND::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnNCMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
		LRESULT TinyVisualHWND::OnNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;

			return FALSE;
		}
		LRESULT TinyVisualHWND::OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled = FALSE;
			return FALSE;
		}
	}
}
