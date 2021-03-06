#include "../stdafx.h"
#include "TinyCanvas.h"

namespace TinyUI
{
	static inline INT GDI_ROUND(DOUBLE val)
	{
		return (INT)floor(val + 0.5);
	}

	RGBQUAD RGBtoHSL(RGBQUAD lRGBColor)
	{
		BYTE R, G, B;
		BYTE H, L, S;
		BYTE cMax, cMin;
		WORD Rdelta, Gdelta, Bdelta;

		R = lRGBColor.rgbRed;
		G = lRGBColor.rgbGreen;
		B = lRGBColor.rgbBlue;

		cMax = max(max(R, G), B);
		cMin = min(min(R, G), B);
		L = (BYTE)((((cMax + cMin)*HSLMAX) + RGBMAX) / (2 * RGBMAX));

		if (cMax == cMin)
		{
			S = 0;
			H = HSLUNDEFINED;
		}
		else
		{
			if (L <= (HSLMAX / 2))
				S = (BYTE)((((cMax - cMin)*HSLMAX) + ((cMax + cMin) / 2)) / (cMax + cMin));
			else
				S = (BYTE)((((cMax - cMin)*HSLMAX) + ((2 * RGBMAX - cMax - cMin) / 2)) / (2 * RGBMAX - cMax - cMin));
			/* hue */
			Rdelta = (WORD)((((cMax - R)*(HSLMAX / 6)) + ((cMax - cMin) / 2)) / (cMax - cMin));
			Gdelta = (WORD)((((cMax - G)*(HSLMAX / 6)) + ((cMax - cMin) / 2)) / (cMax - cMin));
			Bdelta = (WORD)((((cMax - B)*(HSLMAX / 6)) + ((cMax - cMin) / 2)) / (cMax - cMin));

			if (R == cMax)
				H = (BYTE)(Bdelta - Gdelta);
			else if (G == cMax)
				H = (BYTE)((HSLMAX / 3) + Rdelta - Bdelta);
			else
				H = (BYTE)(((2 * HSLMAX) / 3) + Gdelta - Rdelta);

			if (H > HSLMAX) H -= HSLMAX;
		}

		RGBQUAD hsl = { L, S, H, 0 };
		return hsl;
	}
	FLOAT HUEtoRGB(FLOAT n1, FLOAT n2, FLOAT hue)
	{
		FLOAT rValue;

		if (hue > 360)
			hue = hue - 360;
		else if (hue < 0)
			hue = hue + 360;

		if (hue < 60)
			rValue = n1 + (n2 - n1)*hue / 60.0f;
		else if (hue < 180)
			rValue = n2;
		else if (hue < 240)
			rValue = n1 + (n2 - n1)*(240 - hue) / 60;
		else
			rValue = n1;

		return rValue;
	}
	RGBQUAD HSLtoRGB(RGBQUAD lHSLColor)
	{
		FLOAT h, s, l;
		FLOAT m1, m2;
		BYTE r, g, b;

		h = (FLOAT)lHSLColor.rgbRed * 360.0f / 255.0f;
		s = (FLOAT)lHSLColor.rgbGreen / 255.0f;
		l = (FLOAT)lHSLColor.rgbBlue / 255.0f;

		if (l <= 0.5) m2 = l * (1 + s);
		else m2 = l + s - l*s;

		m1 = 2 * l - m2;

		if (s == 0)
		{
			r = g = b = (BYTE)(l * 255.0f);
		}
		else
		{
			r = (BYTE)(HUEtoRGB(m1, m2, h + 120) * 255.0f);
			g = (BYTE)(HUEtoRGB(m1, m2, h) * 255.0f);
			b = (BYTE)(HUEtoRGB(m1, m2, h - 120) * 255.0f);
		}
		RGBQUAD rgb = { b, g, r, 0 };
		return rgb;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyCanvas::TinyCanvas()
		:m_hDC(NULL),
		m_hPEN(NULL),
		m_hBRUSH(NULL),
		m_hFONT(NULL),
		m_iSave(0)
	{
		InitializeDC(m_hDC);
	}
	TinyCanvas::TinyCanvas(HDC hDC)
		:m_hDC(hDC),
		m_hPEN(NULL),
		m_hBRUSH(NULL),
		m_hOldPEN(NULL),
		m_hOldFONT(NULL),
		m_hOldBRUSH(NULL),
		m_iSave(0)
	{
		InitializeDC(m_hDC);
	}
	TinyCanvas::~TinyCanvas()
	{
		if (m_hDC != NULL)
		{
			if (m_hOldBRUSH != NULL)
				SelectObject(m_hDC, m_hOldBRUSH);
			if (m_hOldFONT != NULL)
				SelectObject(m_hDC, m_hOldFONT);
			if (m_hOldPEN != NULL)
				SelectObject(m_hDC, m_hOldPEN);
			RestoreDC(m_hDC, m_iSave);
		}
	}
	TinyCanvas::operator HDC() const
	{
		return m_hDC;
	}
	BOOL TinyCanvas::InitializeDC(HDC hDC)
	{
		m_hDC = hDC;
		m_iSave = SaveDC(m_hDC);
		BOOL res = GetWorldTransform(m_hDC, &m_matrix);
		if (!res) return FALSE;
		m_hPEN = (HPEN)GetCurrentObject(m_hDC, OBJ_PEN);
		m_hBRUSH = (HBRUSH)GetCurrentObject(m_hDC, OBJ_BRUSH);
		SetBkMode(hDC, TRANSPARENT);
		return TRUE;
	}
	HDC TinyCanvas::Handle() const
	{
		return m_hDC;
	}
	HPEN TinyCanvas::SetPen(HPEN hPen)
	{
		if (m_hPEN != hPen)
		{
			m_hPEN = hPen;
			m_hOldPEN = (HPEN)SelectObject(m_hDC, hPen);
		}
		return m_hPEN;
	}
	HBRUSH TinyCanvas::SetBrush(HBRUSH hBrush)
	{
		if (m_hBRUSH != hBrush)
		{
			m_hBRUSH = hBrush;
			m_hOldBRUSH = (HBRUSH)SelectObject(m_hDC, hBrush);
		}
		return m_hBRUSH;
	}
	HFONT TinyCanvas::SetFont(HFONT hFont)
	{
		if (m_hFONT != hFont)
		{
			m_hFONT = hFont;
			m_hOldFONT = (HFONT)SelectObject(m_hDC, m_hFONT);
		}
		return m_hFONT;
	}
	BOOL TinyCanvas::DrawString(const TinyString& str, LPRECT lprc, UINT format)
	{
		if (!m_hDC)
			return FALSE;
		return ::DrawTextEx(m_hDC, str.STR(), str.GetSize(), lprc, format, NULL);
	}
	BOOL TinyCanvas::DrawImage(TinyImage& image, INT x, INT y)
	{
		if (!m_hDC)
			return FALSE;
		TinyMemDC menDC(m_hDC, image);
		TinyRectangle src = image.GetRectangle();
		TinyRectangle dst = src;
		dst.OffsetRect(x, y);
		return menDC.Render(dst, src, TRUE);
	}
	BOOL TinyCanvas::DrawImage(TinyImage& image, INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC)
			return FALSE;
		TinyMemDC menDC(m_hDC, image);
		TinyRectangle src = image.GetRectangle();
		TinyRectangle dst;
		dst.SetRect(x, y, x + cx, y + cy);
		return menDC.Render(dst, src, TRUE);
	}
	BOOL TinyCanvas::DrawImage(TinyImage& image, const RECT& destRect, INT srcX, INT srcY, INT srcCX, INT srcCY)
	{
		if (!m_hDC)
			return FALSE;
		TinyMemDC menDC(m_hDC, image);
		TinyRectangle src;
		src.SetRect(srcX, srcY, srcX + srcCX, srcY + srcCY);
		return menDC.Render(destRect, src, TRUE);
	}
	BOOL TinyCanvas::DrawImage(TinyImage& image, const RECT& destRect, const RECT& srcRect)
	{
		if (!m_hDC)
			return FALSE;
		TinyMemDC menDC(m_hDC, image);
		return menDC.Render(destRect, srcRect, TRUE);
	}
	BOOL TinyCanvas::DrawImage(TinyImage& image, const RECT& dstPaint, const RECT& srcPaint, const RECT& srcCenter)
	{
		if (!m_hDC)
			return FALSE;
		TinyMemDC menDC(m_hDC, image);
		return menDC.Render(dstPaint, srcPaint, srcCenter, TRUE);
	}
	BOOL TinyCanvas::DrawImage(TinyImage& image, const RECT& dstPaint, const RECT& dstCenter, const RECT& srcPaint, const RECT& srcCenter)
	{
		if (!m_hDC)
			return FALSE;
		TinyMemDC menDC(m_hDC, image);
		return menDC.Render(dstPaint, dstCenter, srcPaint, srcCenter, TRUE);
	}

	BOOL TinyCanvas::DrawBitmap(HBITMAP hBitmap, const RECT& srcRect, INT x, INT y)
	{
		if (!m_hDC || !hBitmap)
			return FALSE;
		TinyMemDC menDC(m_hDC, hBitmap);
		TinyRectangle src = srcRect;
		TinyRectangle dst = src;
		dst.OffsetRect(x, y);
		return menDC.Render(dst, src, TRUE);
	}

	BOOL TinyCanvas::DrawBitmap(HBITMAP hBitmap, const RECT& srcRect, INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC || !hBitmap)
			return FALSE;
		TinyMemDC menDC(m_hDC, hBitmap);
		TinyRectangle src = srcRect;
		TinyRectangle dst;
		dst.SetRect(x, y, x + cx, y + cy);
		return menDC.Render(dst, src, TRUE);
	}

	BOOL TinyCanvas::DrawBitmap(HBITMAP hBitmap, const RECT& destRect, const RECT& srcRect)
	{
		if (!m_hDC || !hBitmap)
			return FALSE;
		TinyMemDC menDC(m_hDC, hBitmap);
		return menDC.Render(destRect, srcRect, TRUE);
	}

	BOOL TinyCanvas::DrawBitmap(HBITMAP hBitmap, const RECT& dstPaint, const RECT& srcPaint, const RECT& srcCenter)
	{
		if (!m_hDC || !hBitmap)
			return FALSE;
		TinyMemDC menDC(m_hDC, hBitmap);
		return menDC.Render(dstPaint, srcPaint, srcCenter, TRUE);
	}

	BOOL TinyCanvas::DrawBitmap(HBITMAP hBitmap, const RECT& dstPaint, const RECT& dstCenter, const RECT& srcPaint, const RECT& srcCenter)
	{
		if (!m_hDC || !hBitmap)
			return FALSE;
		TinyMemDC menDC(m_hDC, hBitmap);
		return menDC.Render(dstPaint, dstCenter, srcPaint, srcCenter, TRUE);
	}

	BOOL TinyCanvas::DrawLine(INT sx, INT sy, INT dx, INT dy)
	{
		if (!m_hDC)
			return FALSE;
		POINT ps;
		if (MoveToEx(m_hDC, sx, sy, &ps))
			return LineTo(m_hDC, dx, dy);
		return FALSE;
	}
	BOOL TinyCanvas::DrawLine(POINT pt1, POINT pt2)
	{
		if (!m_hDC) return FALSE;
		POINT ps;
		if (MoveToEx(m_hDC, pt1.x, pt1.y, &ps))
			return LineTo(m_hDC, pt2.x, pt2.y);
		return FALSE;
	}
	BOOL TinyCanvas::DrawLines(POINT* pts, INT size)
	{
		if (!m_hDC)
			return FALSE;
		return Polyline(m_hDC, pts, size);
	}
	BOOL TinyCanvas::DrawPolygon(POINT* pts, INT size)
	{
		if (!m_hDC) return FALSE;
		return Polygon(m_hDC, pts, size);
	}
	BOOL TinyCanvas::DrawArc(INT x, INT y, INT radius, FLOAT startAngle, FLOAT sweepAngle)
	{
		if (!m_hDC)
			return FALSE;
		POINT pt;
		INT x1 = GDI_ROUND(x + cos(startAngle * M_PI / 180) * radius);
		INT y1 = GDI_ROUND(y - sin(startAngle * M_PI / 180) * radius);
		if (MoveToEx(m_hDC, x1, y1, &pt))
		{
			return AngleArc(m_hDC, x, y, radius, startAngle, sweepAngle);
		}
		return FALSE;
	}
	BOOL TinyCanvas::DrawArc(INT x, INT y, INT cx, INT cy, FLOAT startAngle, FLOAT sweepAngle)
	{
		if (!m_hDC)
			return FALSE;
		INT x1, x2, y1, y2;
		DOUBLE tancx1 = cy / tan(startAngle * M_PI / 180);
		DOUBLE tancy1 = cx * tan(startAngle * M_PI / 180);
		x1 = tancy1 <= cy ? (x + cx) : GDI_ROUND(tancy1 + x);
		y1 = tancy1 <= cy ? GDI_ROUND(y + tancy1) : (y + cy);
		DOUBLE tancx2 = cy / tan((startAngle + sweepAngle)*M_PI / 180);
		DOUBLE tancy2 = cx * tan((startAngle + sweepAngle)*M_PI / 180);
		x2 = tancy2 <= cy ? (x + cx) : GDI_ROUND(tancy2 + x);
		y2 = tancy2 <= cy ? GDI_ROUND(y + tancy2) : (y + cy);
		INT iArc = SetArcDirection(m_hDC, sweepAngle >= 0 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE);
		BOOL bRes = Arc(m_hDC, x, y, x + cx, y + cy, x1, y1, x2, y2);
		SetArcDirection(m_hDC, iArc);
		return TRUE;
	}
	BOOL TinyCanvas::DrawPie(INT x, INT y, INT radius, FLOAT startAngle, FLOAT sweepAngle)
	{
		if (!m_hDC)
			return FALSE;
		INT x1 = GDI_ROUND(x + cos(startAngle * M_PI / 180) * radius);
		INT y1 = GDI_ROUND(y - sin(startAngle * M_PI / 180) * radius);
		INT x2 = GDI_ROUND(x + cos((startAngle + sweepAngle) * M_PI / 180) * radius);
		INT y2 = GDI_ROUND(y - sin((startAngle + sweepAngle) * M_PI / 180) * radius);
		INT iArc = SetArcDirection(m_hDC, sweepAngle >= 0 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE);
		BOOL bRes = Pie(m_hDC, x - radius, y - radius, x + radius, y + radius, x1, y1, x2, y2);
		SetArcDirection(m_hDC, iArc);
		return bRes;
	}
	BOOL TinyCanvas::DrawPie(INT x, INT y, INT cx, INT cy, FLOAT startAngle, FLOAT sweepAngle)
	{
		if (!m_hDC)
			return FALSE;
		INT x1, x2, y1, y2;
		DOUBLE tancx1 = cy / tan(startAngle * M_PI / 180);
		DOUBLE tancy1 = cx * tan(startAngle * M_PI / 180);
		x1 = tancy1 <= cy ? (x + cx) : GDI_ROUND(tancy1 + x);
		y1 = tancy1 <= cy ? GDI_ROUND(y + tancy1) : (y + cy);
		DOUBLE tancx2 = cy / tan((startAngle + sweepAngle)*M_PI / 180);
		DOUBLE tancy2 = cx * tan((startAngle + sweepAngle)*M_PI / 180);
		x2 = tancy2 <= cy ? (x + cx) : GDI_ROUND(tancy2 + x);
		y2 = tancy2 <= cy ? GDI_ROUND(y + tancy2) : (y + cy);
		INT iArc = SetArcDirection(m_hDC, sweepAngle >= 0 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE);
		BOOL bRes = Pie(m_hDC, x, y, x + cx, y + cy, x1, y1, x2, y2);
		SetArcDirection(m_hDC, iArc);
		return TRUE;
	}
	BOOL TinyCanvas::DrawRectangle(const RECT& rect)
	{
		if (!m_hDC)
			return FALSE;
		HGDIOBJ hOld = ::SelectObject(m_hDC, GetStockObject(NULL_BRUSH));
		BOOL bRes = ::Rectangle(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
		::SelectObject(m_hDC, hOld);
		return bRes;
	}
	BOOL TinyCanvas::DrawRectangle(INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC)
			return FALSE;
		HGDIOBJ hOld = ::SelectObject(m_hDC, GetStockObject(NULL_BRUSH));
		BOOL bRes = ::Rectangle(m_hDC, x, y, x + cx, y + cy);
		::SelectObject(m_hDC, hOld);
		return bRes;
	}
	BOOL TinyCanvas::DrawBeziers(POINT* pts, INT size)
	{
		if (!m_hDC)
			return FALSE;
		return PolyBezier(m_hDC, pts, size);
	}
	BOOL TinyCanvas::DrawEllipse(const RECT& rect)
	{
		if (!m_hDC)
			return FALSE;
		return Ellipse(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
	}
	BOOL TinyCanvas::DrawEllipse(INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC)
			return FALSE;
		return Ellipse(m_hDC, x, y, x + cx, y + cy);
	}
	BOOL TinyCanvas::FillPie(INT x, INT y, INT cx, INT cy, FLOAT startAngle, FLOAT sweepAngle)
	{
		DWORD radius = (DWORD)sqrt(cx * cx + cy * cy);
		INT x1 = GDI_ROUND(x + cos(startAngle * M_PI / 180) * radius);
		INT y1 = GDI_ROUND(y - sin(startAngle * M_PI / 180) * radius);
		INT x2 = GDI_ROUND(x + cos((startAngle + sweepAngle) * M_PI / 180) * radius);
		INT y2 = GDI_ROUND(y - sin((startAngle + sweepAngle) * M_PI / 180) * radius);
		INT iArc = SetArcDirection(m_hDC, sweepAngle >= 0 ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE);
		BOOL bRes = Pie(m_hDC, x - radius, y - radius, x + radius, y + radius, x1, y1, x2, y2);
		SetArcDirection(m_hDC, iArc);
		return bRes;
	}
	BOOL TinyCanvas::FillPolygon(POINT* pts, INT size)
	{
		if (!m_hDC)
			return FALSE;
		return Polygon(m_hDC, pts, size);
	}
	BOOL TinyCanvas::FillEllipse(INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC)
			return FALSE;
		return Ellipse(m_hDC, x, y, x + cx, y + cy);
	}
	BOOL TinyCanvas::FillEllipse(const RECT& rect)
	{
		if (!m_hDC) return FALSE;
		return Ellipse(m_hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
	BOOL TinyCanvas::FillRectangle(const RECT& rect)
	{
		if (!m_hDC)
			return FALSE;
		return PatBlt(m_hDC, rect.left, rect.top, abs(rect.right - rect.left), abs(rect.bottom - rect.top), PATCOPY);
	}
	BOOL TinyCanvas::FillRectangle(INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC)
			return FALSE;
		return PatBlt(m_hDC, x, y, cx, cy, PATCOPY);
	}
	BOOL TinyCanvas::FillRegion(HRGN hRgn)
	{
		if (!m_hDC || !hRgn)
			return FALSE;
		return PaintRgn(m_hDC, hRgn);
	}
	BOOL TinyCanvas::GradientFill(LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz)
	{
		if (!m_hDC)
			return FALSE;
		TRIVERTEX vert[2];
		vert[0].x = lpRect->left;
		vert[0].y = lpRect->top;
		vert[0].Red = (COLOR16)(GetRValue(crFrom) << 8);
		vert[0].Green = (COLOR16)(GetGValue(crFrom) << 8);
		vert[0].Blue = (COLOR16)(GetBValue(crFrom) << 8);
		vert[0].Alpha = 0x0000;
		vert[1].x = lpRect->right;
		vert[1].y = lpRect->bottom;
		vert[1].Red = (COLOR16)(GetRValue(crTo) << 8);
		vert[1].Green = (COLOR16)(GetGValue(crTo) << 8);
		vert[1].Blue = (COLOR16)(GetBValue(crTo) << 8);
		vert[1].Alpha = 0x0000;
		GRADIENT_RECT gRect = { 0, 1 };
		return ::GradientFill(m_hDC, vert, 2, &gRect, 1, bHorz ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V);
	}
	BOOL TinyCanvas::SetClip(const RECT& rect)
	{
		if (!m_hDC) return FALSE;
		HRGN hRgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
		if (hRgn != NULL)
		{
			SelectClipRgn(m_hDC, hRgn);
			SAFE_DELETE_OBJECT(hRgn);
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyCanvas::SetClip(HRGN hRgn)
	{
		if (!m_hDC || !hRgn) return FALSE;
		SelectClipRgn(m_hDC, hRgn);
		return TRUE;
	}
	BOOL TinyCanvas::SetClip(INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC) return FALSE;
		HRGN hRgn = CreateRectRgn(x, y, x + cx, y + cy);
		if (hRgn != NULL)
		{
			SelectClipRgn(m_hDC, hRgn);
			SAFE_DELETE_OBJECT(hRgn);
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyCanvas::SetClip(const RECT& rect, INT iCombineMode)
	{
		if (!m_hDC) return FALSE;
		HRGN hRgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
		if (hRgn != NULL)
		{
			ExtSelectClipRgn(m_hDC, hRgn, iCombineMode);
			SAFE_DELETE_OBJECT(hRgn);
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyCanvas::SetClip(HRGN hRgn, INT iCombineMode)
	{
		if (!m_hDC || !hRgn) return FALSE;
		ExtSelectClipRgn(m_hDC, hRgn, iCombineMode);
		return TRUE;
	}
	BOOL TinyCanvas::SetClip(INT x, INT y, INT cx, INT cy, INT iCombineMode)
	{
		if (!m_hDC) return FALSE;
		HRGN hRgn = CreateRectRgn(x, y, x + cx, y + cy);
		if (hRgn != NULL)
		{
			ExtSelectClipRgn(m_hDC, hRgn, iCombineMode);
			SAFE_DELETE_OBJECT(hRgn);
			return TRUE;
		}
		return FALSE;
	}
	BOOL TinyCanvas::ResetClip()
	{
		if (!m_hDC) return FALSE;
		return ExtSelectClipRgn(m_hDC, NULL, RGN_COPY) != ERROR;
	}
	BOOL TinyCanvas::IntersectClip(const RECT& rect)
	{
		if (!m_hDC) return FALSE;
		return IntersectClipRect(m_hDC, rect.left, rect.top, rect.right, rect.bottom) != ERROR;
	}
	BOOL TinyCanvas::IntersectClip(INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC) return FALSE;
		return IntersectClipRect(m_hDC, x, y, x + cx, y + cy) != ERROR;
	}
	BOOL TinyCanvas::IntersectClip(HRGN hRgn)
	{
		if (!m_hDC) return FALSE;
		HRGN hNewRgn = CreateRectRgn(0, 0, 0, 0);
		if (!hNewRgn) goto LABEL;
		if (GetClipRgn(m_hDC, hNewRgn) != 1)
			goto LABEL;
		if (CombineRgn(hNewRgn, hNewRgn, hRgn, RGN_AND) == ERROR)
			goto LABEL;
		SAFE_DELETE_OBJECT(hNewRgn);
		return TRUE;
	LABEL:
		SAFE_DELETE_OBJECT(hNewRgn);
		return FALSE;
	}
	BOOL TinyCanvas::ExcludeClip(const RECT& rect)
	{
		if (!m_hDC) return FALSE;
		return ExcludeClipRect(m_hDC, rect.left, rect.top, rect.right, rect.bottom);
	}
	BOOL TinyCanvas::ExcludeClip(INT x, INT y, INT cx, INT cy)
	{
		if (!m_hDC) return FALSE;
		return ExcludeClipRect(m_hDC, x, y, x + cx, y + cy);
	}
	BOOL TinyCanvas::ExcludeClip(HRGN hRgn)
	{
		if (!m_hDC) return FALSE;
		HRGN hNewRgn = CreateRectRgn(0, 0, 0, 0);
		if (!hNewRgn) goto LABEL;
		if (GetClipRgn(m_hDC, hNewRgn) != 1)
			goto LABEL;
		if (CombineRgn(hNewRgn, hNewRgn, hRgn, RGN_DIFF) == ERROR)
			goto LABEL;
		SAFE_DELETE_OBJECT(hNewRgn);
		return TRUE;
	LABEL:
		SAFE_DELETE_OBJECT(hNewRgn);
		return FALSE;
	}
	BOOL TinyCanvas::IsClipEmpty() const
	{
		if (!m_hDC) return FALSE;
		HRGN hNewRgn = CreateRectRgn(0, 0, 0, 0);
		BOOL bRes = GetClipRgn(m_hDC, hNewRgn) == 0;
		SAFE_DELETE_OBJECT(hNewRgn);
		return bRes;
	}
	BOOL TinyCanvas::IsVisibleClipEmpty() const
	{
		if (!m_hDC) return FALSE;
		RECT rect = { 0 };
		return GetClipBox(m_hDC, &rect) == NULLREGION;
	}
	BOOL TinyCanvas::IsVisible(POINT pt)
	{
		if (!m_hDC) return FALSE;
		return PtVisible(m_hDC, pt.x, pt.y);
	}
	BOOL TinyCanvas::IsVisible(const RECT& rect)
	{
		if (!m_hDC) return FALSE;
		return RectVisible(m_hDC, &rect);
	}
	INT TinyCanvas::GetClipRectangle(LPRECT lprect)
	{
		if (!m_hDC) return FALSE;
		return ::GetClipBox(m_hDC, lprect) != ERROR;
	}
	BOOL TinyCanvas::TranslateTransform(FLOAT x, FLOAT y)
	{
		if (!m_hDC) return FALSE;
		m_matrix.SetTranslate(x, y);
		return SetWorldTransform(m_hDC, &m_matrix);
	}
	BOOL TinyCanvas::RotateTransform(FLOAT angle)
	{
		if (!m_hDC) return FALSE;
		m_matrix.SetRotate(angle);
		return SetWorldTransform(m_hDC, &m_matrix);
	}
	BOOL TinyCanvas::ScaleTransform(FLOAT sx, FLOAT sy)
	{
		if (!m_hDC) return FALSE;
		m_matrix.SetScale(sx, sy);
		return SetWorldTransform(m_hDC, &m_matrix);
	}
	BOOL TinyCanvas::ShearTransform(FLOAT sh, FLOAT sv)
	{
		if (!m_hDC) return FALSE;
		m_matrix.SetShear(sh, sv);
		return SetWorldTransform(m_hDC, &m_matrix);
	}
	BOOL TinyCanvas::ResetTransform()
	{
		if (!m_hDC) return FALSE;
		m_matrix.Reset();
		return SetWorldTransform(m_hDC, &m_matrix);
	}

	COLORREF  TinyCanvas::SetTextColor(COLORREF color)
	{
		ASSERT(m_hDC);
		return ::SetTextColor(m_hDC, color);
	}

	UINT TinyCanvas::SetTextAlign(UINT align)
	{
		ASSERT(m_hDC);
		return ::SetTextAlign(m_hDC, align);
	}
};
