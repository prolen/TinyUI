#include "../stdafx.h"
#include "TinyVisualHWND.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualRichText.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}

		HRESULT TinyVisualRichText::OnMouseMove(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			m_document->GetVisualHWND()->SetMsgHandled(FALSE);
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			LRESULT lRes = FALSE;
			m_document->GetVisualHWND()->SetMsgHandled(m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes) == S_OK);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage)
		{
			ASSERT(m_texthost.m_ts);
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			POINT pos;
			GetCursorPos(&pos);
			::ScreenToClient(hWND, &pos);
			HDC hDC = GetDC(hWND);
			if (hDC != NULL)
			{
				TinyRectangle clip = GetWindowRect();
				m_texthost.m_ts->OnTxSetCursor(DVASPECT_CONTENT, -1, NULL, NULL, hDC, NULL, &clip, pos.x, pos.y);
				ReleaseDC(hWND, hDC);
			}
			return TRUE;
		}

		HRESULT	TinyVisualRichText::OnSetFocus()
		{
			ASSERT(m_texthost.m_ts);
			TinyRectangle clip = GetWindowRect();
			m_texthost.m_ts->OnTxInPlaceActivate(clip);
			m_texthost.m_ts->OnTxUIActivate();
			m_texthost.m_ts->TxSendMessage(WM_SETFOCUS, 0, 0, NULL);
			return FALSE;
		}

		HRESULT	TinyVisualRichText::OnKillFocus()
		{
			ASSERT(m_texthost.m_ts);
			m_texthost.m_ts->OnTxInPlaceDeactivate();
			m_texthost.m_ts->OnTxUIDeactivate();
			m_texthost.m_ts->TxSendMessage(WM_KILLFOCUS, 0, 0, NULL);
			return FALSE;
		}

		HRESULT TinyVisualRichText::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
		{
			ASSERT(m_texthost.m_ts);
			return m_texthost.m_ts->TxSendMessage(uMsg, wParam, lParam, &lRes);
		}

		TinyVisualRichText::~TinyVisualRichText()
		{

		}
		TinyString TinyVisualRichText::RetrieveTag() const
		{
			return TinyVisualTag::RICHTEXT;
		}

		BOOL TinyVisualRichText::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_texthost.m_ts);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			::SetGraphicsMode(canvas, GM_COMPATIBLE);
			TinyRectangle clip = GetWindowRect();
			m_texthost.m_ts->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, canvas, NULL, reinterpret_cast<LPCRECTL>(&clip), NULL, NULL, NULL, 0, 0);
			return TRUE;
		}
		HRESULT TinyVisualRichText::OnCreate()
		{
			m_texthost.Initialize(this);
			m_texthost.UpdateView();
			return S_OK;
		}
		HRESULT TinyVisualRichText::OnDestory()
		{
			return S_OK;
		}
	}
}
