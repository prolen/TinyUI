#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class DX11RenderView;

	class DX11 : public TinyLock
	{
		friend class DX11Graphics2D;
		DISALLOW_COPY_AND_ASSIGN(DX11)
	public:
		DX11();
		virtual ~DX11();
		BOOL					EnumAdapters(vector<IDXGIAdapter*>& adapters);
		BOOL					Initialize(HWND hWND, INT cx, INT cy, IDXGIAdapter* pAdapter = NULL);
		void					Uninitialize();
		BOOL					ResizeView(INT cx = 0, INT cy = 0);
		BOOL					SetViewport(const TinyPoint& pos, const TinySize& size);
		BOOL					AllowBlend(BOOL bAllow, FLOAT blendFactor[4]);
		BOOL					AllowDepth(BOOL bAllow);
		BOOL					Present();
		BOOL					Flush();
		void					SetMatrixs(const TinySize& size);
	public:
		HWND					GetHWND() const;
		BOOL					IsEmpty() const;
		ID3D11Device*			GetD3D() const;
		IDXGISwapChain*			GetSwapChain() const;
		ID3D11DeviceContext*	GetImmediateContext() const;
		XMMATRIX*				GetMatrixs();
		TinySize				GetSize() const;
	private:
		void					SetRenderView(DX11RenderView* render2D);
		DX11RenderView*			GetRenderView() const;
	private:
		HWND								m_hWND;
		DX11RenderView*						m_render2D;
		XMMATRIX							m_matrixs[3];
		TinySize							m_size;
		TinyComPtr<ID3D11Device>			m_d3d;
		TinyComPtr<IDXGISwapChain>			m_swap;
		TinyComPtr<ID3D11DeviceContext>		m_immediateContext;
		TinyComPtr<ID3D11RasterizerState>	m_rasterizerState;
		TinyComPtr<ID3D11BlendState>		m_enableBlendState;
		TinyComPtr<ID3D11BlendState>		m_disableBlendState;
		TinyComPtr<ID3D11DepthStencilState>	m_enableDepthState;
		TinyComPtr<ID3D11DepthStencilState>	m_disableDepthState;
		TinyScopedPtr<DX11RenderView>		m_background2D;
	};
}

