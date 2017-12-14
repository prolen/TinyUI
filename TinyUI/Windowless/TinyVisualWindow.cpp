#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualWindow, TinyVisual);

		TinyVisualWindow::TinyVisualWindow()
		{

		}
		TinyVisualWindow::TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
		{

		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}

		void TinyVisualWindow::OnSizeChange(const TinySize& oldsize, const TinySize& newsize)
		{
			TinyVisual* spvis = m_spvisChild;
			while (spvis != NULL && spvis->IsVisible())
			{
				TinySize size = spvis->GetSize();
				spvis->SetSize(TinySize(newsize.cx, size.cy));
				spvis->OnSizeChange(size, spvis->GetSize());
				spvis = m_document->GetVisual(spvis, CMD_NEXT);
			}
		}

		void TinyVisualWindow::SetPosition(const TinyPoint& pos)
		{
			TinyVisual::SetPosition(TinyPoint(0, 0));
		}

		void TinyVisualWindow::SetText(const TinyString& pzText)
		{
			ASSERT(m_document || m_document->GetVisualHWND());
			::SetWindowText(m_document->GetVisualHWND()->Handle(), pzText.CSTR());
			TinyVisual::SetText(pzText);
		}

		BOOL TinyVisualWindow::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::SHADOWIMAGE.STR()) == 0)
			{
				if (m_document != NULL && m_document->GetVisualHWND() != NULL)
				{
					m_document->GetVisualHWND()->GetShadow().SetShadow(TinyVisualResource::GetInstance()[value]);
				}
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualProperty::SHADOWBOX.STR()) == 0)
			{
				if (m_document != NULL && m_document->GetVisualHWND() != NULL)
				{
					m_document->GetVisualHWND()->GetShadow().SetShadowBox(TinyVisualBuilder::GetRectangle(value));
				}
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}

		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTag::WINDOW;
		}
		BOOL TinyVisualWindow::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			if (!m_document)
				return FALSE;
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (m_borderImage != NULL && !m_borderImage->IsEmpty())
			{
				TinyRectangle srcRect = m_borderImage->GetRectangle();
				TinyRectangle srcCenter = GetBorderCenter();
				if (srcCenter.IsRectEmpty())
				{
					canvas.DrawImage(*m_borderImage, clip, srcRect);
				}
				else
				{
					canvas.DrawImage(*m_borderImage, clip, srcRect, srcCenter);
				}
			}
			if (!m_borderColor.IsEmpty() && m_borderThickness != -1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				HPEN hOldPen = canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
				canvas.SetPen(hOldPen);
			}
			if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
			{
				TinyRectangle srcRect = m_backgroundImage->GetRectangle();
				TinyRectangle srcCenter = GetBackgroundCenter();
				if (srcCenter.IsRectEmpty())
				{
					canvas.DrawImage(*m_backgroundImage, clip, srcRect);
				}
				else
				{
					canvas.DrawImage(*m_backgroundImage, clip, srcRect, srcCenter);
				}
			}
			if (!m_backgroundColor.IsEmpty())
			{
				TinyBrush brush;
				brush.CreateBrush(m_backgroundColor);
				HBRUSH hOldBrush = canvas.SetBrush(brush);
				canvas.FillRectangle(clip);
				canvas.SetBrush(hOldBrush);
			}	
			return TRUE;
		}
	}
}
