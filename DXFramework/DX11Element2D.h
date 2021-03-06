#pragma once
#include "DXFramework.h"
#include "DX11Texture2D.h"
#include "DX11TextureShader.h"
#include "Render/TinyRectTracker.h"
using namespace TinyUI;

namespace DXFramework
{
	/// <summary>
	/// DX11Ԫ��
	/// </summary>
	class DX11Element2D : public TinyObject, public TinyRectTracker
	{
		DECLARE_DYNAMIC(DX11Element2D)
	public:
		DX11Element2D();
		virtual ~DX11Element2D();
	public:
		TinyString GetName() const;
		TinyPoint GetPosition() const;
		TinySize GetScale() const;
		TinySize GetSize() const;
		void	SetPosition(const TinyPoint& pos);
		void	SetScale(const TinySize& size);
		void	SetSize(const TinySize& size);
		void	SetName(const TinyString& name);
		BOOL	PtInRect(const TinyPoint& pos);
	public:
		virtual BOOL Allocate(DX11& dx11) = 0;
		virtual BOOL Process(DX11& dx11) = 0;
		virtual void Deallocate(DX11& dx11) = 0;
	public:
		TinyString		m_name;
		TinySize		m_size;
	};
}



