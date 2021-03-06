#pragma once
#include "RenderTask.h"
#include "AudioEncode.h"
#include "VideoEncode.h"
#include "PublishTask.h"
#include "DXWindow.h"
#include "MainUI.h"
using namespace DXFramework;

namespace DXApp
{
	class DXFrameUI : public TinyControl
	{
		DECLARE_DYNAMIC(DXFrameUI)
	public:
		DXFrameUI();
		virtual ~DXFrameUI();
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		HICON RetrieveIcon() OVERRIDE;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	public:
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	private:
		BOOL CreateUI();
		void Resize(INT cx, INT cy);
	private:
		DXWindow						m_window;
		MainUI							m_mainUI;
	};

}



