#pragma once
#include "../IO/TinyIO.h"
#include "TinyGDI.h"
#include <d2d1.h>
#include <Wincodec.h>
#pragma comment(lib,"Windowscodecs.lib")

namespace TinyUI
{
	/// <summary>
	/// 图片加载类
	/// </summary>
	class TinyImage
	{
		DISALLOW_COPY_AND_ASSIGN(TinyImage)
	public:
		TinyImage();
		virtual ~TinyImage();
		operator HBITMAP() const;
		BOOL			IsEmpty() const;
		BOOL			Open(LPCSTR pz);
		BOOL			Open(BYTE* p, LONG size);
		BOOL			Save(LPCSTR pz);//保存成BMP
		HBITMAP			GetHBITMAP();
		BYTE*			GetBits();
		TinySize		GetSize();
		TinyRectangle	GetRectangle();
		void			Close();
	private:
		BOOL			OpenFile(LPCSTR pzFile);
	protected:
		INT				m_cx;
		INT				m_cy;
		BYTE*			m_bits;
		HBITMAP			m_hBitmap;
	};
	/// <summary>
	/// 微软WIC解码
	/// </summary>
	class TinyWICImage
	{
		DISALLOW_COPY_AND_ASSIGN(TinyWICImage)
	public:
		TinyWICImage();
		virtual ~TinyWICImage();
		operator HBITMAP() const;
		BOOL			IsEmpty() const;
		BOOL			Open(LPCSTR pz);
		BOOL			Open(BYTE* p, LONG size);
		BOOL			Save(LPCSTR pz);//保存成BMP
		HBITMAP			GetHBITMAP();
		BYTE*			GetBits();
		TinySize		GetSize();
		TinyRectangle	GetRectangle();
		void			Close();
	private:
		BOOL			OpenFile(LPCSTR pzFile);
	protected:
		INT				m_cx;
		INT				m_cy;
		BYTE*			m_bits;
		HBITMAP			m_hBitmap;
	};
}

