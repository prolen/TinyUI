﻿#include "stdafx.h"
#include "MVideoController.h"
#include "MPreviewController.h"
#include "MVideoElement.h"
#include "MShowApp.h"
#include "resource.h"

namespace MShow
{
	MVideoDialog::MVideoDialog()
	{

	}
	MVideoDialog::~MVideoDialog()
	{

	}
	LRESULT MVideoDialog::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			m_address = m_textbox.GetText();
			m_address = m_address.Trim(" ");
			if (EndDialog(LOWORD(wParam)))
			{

			}
			break;
		}
		return TinyCustomDialog::OnCommand(uMsg, wParam, lParam, bHandled);
	}
	LRESULT MVideoDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_textbox.SubclassDlgItem(IDC_EDIT_VIDEO, m_hWND);
		return FALSE;
	}
	TinyString MVideoDialog::GetAddress()
	{
		return m_address;
	}
	//////////////////////////////////////////////////////////////////////////
	MVideoController::MVideoController(MVideoView& view, INT index)
		:m_view(view),
		m_index(index),
		m_pVideo(NULL)
	{
		m_onLButtonDBClick.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MVideoController::OnLButtonDBClick));
		m_onRButtonDown.Reset(new Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>(this, &MVideoController::OnRButtonDown));
		m_view.EVENT_LDBCLICK += m_onLButtonDBClick;
		m_view.EVENT_RBUTTONDOWN += m_onRButtonDown;
		m_onMenuClick.Reset(new Delegate<void(void*, INT)>(this, &MVideoController::OnMenuClick));
		m_popup.EVENT_CLICK += m_onMenuClick;
		m_onVolume.Reset(new Delegate<void(DWORD)>(this, &MVideoController::OnVolume));
		m_event.CreateEvent();
		m_copy.CreateEvent();
		m_signal.CreateEvent();
	}

	MVideoController::~MVideoController()
	{
		m_view.EVENT_LDBCLICK -= m_onLButtonDBClick;
		m_view.EVENT_RBUTTONDOWN -= m_onRButtonDown;
		m_popup.EVENT_CLICK -= m_onMenuClick;
		m_popup.DestroyMenu();
		m_event.Close();
		m_signal.Close();
	}

	BOOL MVideoController::Initialize()
	{
		TinyRectangle rectangle;
		m_view.GetClientRect(&rectangle);
		if (!m_graphics.Initialize(m_view.Handle(), rectangle.Size()))
			return FALSE;
		m_popup.CreatePopupMenu();
		m_popup.AppendMenu(MF_STRING, 1, TEXT("添加"));
		m_popup.AppendMenu(MF_STRING, 2, TEXT("删除"));
		m_popup.AppendMenu(MF_STRING, 3, TEXT("切流"));
		return TRUE;
	}

	BOOL MVideoController::Open(LPCSTR pzURL)
	{
		TinySize size;
		TinyRectangle rectangle;
		m_player.Reset(new MFLVPlayer(BindCallback(&MVideoController::OnAudio, this), BindCallback(&MVideoController::OnVideoCopy, this), BindCallback(&MVideoController::OnVideoRender, this)));
		if (!m_player || !m_player->Open(m_view.Handle(), pzURL))
			goto _ERROR;
		size = m_player->GetSize();
		if (!m_video2D.Create(m_graphics.GetDX11(), size.cx, size.cy, FALSE))
			goto _ERROR;
		m_view.GetClientRect(&rectangle);
		m_video2D.SetScale(rectangle.Size());
		return TRUE;
	_ERROR:
		if (m_player != NULL)
		{
			m_player->Close();
		}
		m_video2D.Destory();
		return FALSE;
	}

	BOOL MVideoController::Close()
	{
		if (m_player != NULL)
		{
			m_player->Close();
		}
		m_video2D.Destory();
		return TRUE;
	}

	TinyString	MVideoController::GetURL() const
	{
		if (!m_player)
			return TinyString();
		return m_player->GetURL();
	}

	HANDLE MVideoController::GetHandle()
	{
		return m_video2D.GetHandle();
	}

	WAVEFORMATEX* MVideoController::GetFormat()
	{
		if (!m_player)
			return NULL;
		return m_player->GetFormat();
	}
	DX11Element2D* MVideoController::GetElement()
	{
		return m_pVideo;
	}
	void MVideoController::OnAudio(BYTE* bits, LONG size)
	{
		EVENT_AUDIO(bits, size);
	}

	void MVideoController::OnVideoCopy(BYTE* bits, LONG size)
	{
		if (m_player != NULL)
		{
			TinySize videoSize = m_player->GetSize();
			if (m_video2D.Copy(m_graphics.GetDX11(), NULL, bits, size))
			{
				m_event.SetEvent();
				m_graphics.SetRenderView(NULL);
				m_graphics.GetRenderView()->BeginDraw();
				FLOAT blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				m_graphics.GetDX11().AllowBlend(FALSE, blendFactor);
				m_graphics.DrawImage(&m_video2D, 1.0F, 1.0F);
				m_graphics.GetRenderView()->EndDraw();
			}
		}
	}

	void MVideoController::OnVideoRender()
	{
		m_graphics.Present();
	}

	void MVideoController::OnAdd()
	{
		MVideoDialog dlg;
		if (dlg.DoModal(m_view.Handle(), IDD_VIDEO) == IDOK)
		{
			this->OnRemove();
			TinyString szName = dlg.GetAddress();
			if (!szName.IsEmpty())
			{
				this->Open(szName.STR());
			}
		}
	}
	void MVideoController::OnChange()
	{
		MShowApp::GetInstance().GetController().SetCurrentCTRL(this);
	}
	void MVideoController::OnRemove()
	{
		MPreviewController* pCTRL = MShowApp::GetInstance().GetController().GetPreviewController();
		if (pCTRL != NULL && m_pVideo != NULL)
		{
			pCTRL->Remove(m_pVideo);
			m_pVideo->Deallocate(pCTRL->Graphics().GetDX11());
			SAFE_DELETE(m_pVideo);
		}
		this->Close();
		m_view.Invalidate();
	}

	void MVideoController::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		TinyPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		m_view.ClientToScreen(&point);
		m_popup.TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, m_view.Handle(), NULL);
	}

	void MVideoController::OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		AddElement();
	}

	void MVideoController::AddElement()
	{
		MPreviewController* pCTRL = MShowApp::GetInstance().GetController().GetPreviewController();
		if (pCTRL != NULL)
		{
			if (m_pVideo == NULL)
			{
				m_pVideo = new MVideoElement(*this);
				if (m_pVideo->Allocate(pCTRL->Graphics().GetDX11()))
				{
					pCTRL->Add(m_pVideo);
					pCTRL->Bring(m_pVideo, TRUE);
				}
			}
			else
			{
				pCTRL->Add(m_pVideo);
				pCTRL->Bring(m_pVideo, TRUE);
			}
		}
	}

	HANDLE MVideoController::GetEvent()
	{
		return m_event.Handle();
	}

	void MVideoController::OnMenuClick(void*, INT wID)
	{
		switch (wID)
		{
		case 1:
			OnAdd();
			break;
		case 2:
			OnRemove();
			break;
		case 3:
			OnChange();
			break;
		}
	}

	void MVideoController::OnVolume(DWORD pos)
	{
		if (m_player != NULL)
		{
			m_player->SetVolume(pos);
		}
	}
}

