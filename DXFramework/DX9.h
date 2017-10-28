#pragma once
#include "DXFramework.h"
#include <d3d9.h>
#include <d3dx9tex.h>

namespace DXFramework
{
	class DX9 : public TinyLock
	{
		DISALLOW_COPY_AND_ASSIGN(DX9)
	public:
		DX9();
		virtual ~DX9();
		BOOL Initialize(HWND hWND, INT cx, INT cy);
		BOOL Render();
		BOOL Present();
	public:
		HWND				GetHWND() const;
		BOOL				IsEmpty() const;
		IDirect3DDevice9*	GetD3D() const;
	private:
		HWND							m_hWND;
		D3DVIEWPORT9					m_viewPort;
		TinyComPtr<IDirect3D9Ex>		m_d3d9;
		TinyComPtr<IDirect3DDevice9>	m_d3dd9;
	};
}

