#include "stdafx.h"
#include "MFFrame.h"

//#include <mfxjpeg.h>
//#include <mfxmvc.h>
//#include <mfxplugin.h>
//#include <mfxplugin++.h>
//#include <mfxvideo.h>
//#include <mfxvideo++.h>


MFFrame::MFFrame()
{
}


MFFrame::~MFFrame()
{
}

LRESULT MFFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PostQuitMessage(0);
	return FALSE;
}

BOOL MFFrame::Create(HWND hParent, INT x, INT y, INT cx, INT cy)
{
	return TinyControl::Create(hParent, x, y, cx, cy, FALSE);
}
DWORD MFFrame::RetrieveStyle()
{
	return (WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_CAPTION);
}

DWORD MFFrame::RetrieveExStyle()
{
	return (WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR);
}

LPCSTR MFFrame::RetrieveClassName()
{
	return TEXT("ChatFrame");
}

LPCSTR MFFrame::RetrieveTitle()
{
	return TEXT("ChatFrame");
}

HICON MFFrame::RetrieveIcon()
{
	return NULL;
}

LRESULT MFFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	vector<MF::MFVideoCapture::Name> names;
	MF::MFVideoCapture::GetDevices(names);
	vector<MF::MFVideoCaptureParam> params;
	MF::MFVideoCapture::GetDeviceParams(names[0], params);

	/*for (INT i = 0;i < params.size();i++)
	{
		TRACE("%d, desc:%s\n", i, params[i].ToString().c_str());
	}*/

	/*m_param = params[10];
	m_capture.Initialize(names[0], BindCallback(&MFFrame::OnFrame, this));
	m_capture.Allocate(m_param);*/

	return FALSE;
}

LRESULT MFFrame::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	//m_capture.Uninitialize();

	return FALSE;
}

LRESULT MFFrame::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	PAINTSTRUCT s = { 0 };
	HDC hDC = BeginPaint(m_hWND, &s);
	EndPaint(m_hWND, &s);
	return FALSE;
}

LRESULT MFFrame::OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return FALSE;
}

void MFFrame::OnFrame(BYTE* bits, LONG size, FLOAT ts, LPVOID ps)
{
	MF::MFVideoCapture* capture = reinterpret_cast<MF::MFVideoCapture*>(ps);
	ASSERT(capture);
	HDC hDC = GetDC(m_hWND);
	//TinyCanvas canvas(hDC);
	//TinyImage image;
	//image.Load(bits, size);
	//canvas.DrawImage(image, 0, 0, image.GetSize().cx, image.GetSize().cy);
	ReleaseDC(m_hWND, hDC);
}
