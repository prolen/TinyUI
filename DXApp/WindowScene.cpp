#include "stdafx.h"
#include "WindowScene.h"

namespace DXApp
{
	WindowScene::WindowScene()
		:m_hWND(NULL)
	{
	}


	WindowScene::~WindowScene()
	{
	}

	BOOL WindowScene::Initialize(DX11& dx11, HWND hWND)
	{
		m_hWND = hWND;
		Destory();
		TinyRectangle rectangle;
		GetClientRect(m_hWND, &rectangle);
		if (!rectangle.IsRectEmpty())
		{
			return DX11Image::Create(dx11, rectangle.Size(), NULL);
		}
		return FALSE;
	}

	LPCSTR WindowScene::GetClassName()
	{
		return TEXT("WindowScene");
	}

	DXFramework::ElementType WindowScene::GetElementType() const
	{
		return IMAGE;
	}

	BOOL WindowScene::BeginScene()
	{
		return TRUE;
	}

	void WindowScene::EndScene()
	{

	}

	BOOL WindowScene::Render(const DX11& dx11)
	{
		HDC hDC = GetDC(m_hWND);
		if (hDC != NULL)
		{
			TinyRectangle rectangle;
			GetClientRect(m_hWND, &rectangle);
			this->BitBlt(dx11, hDC, rectangle);
			DX11Image::Render(dx11);
			ReleaseDC(m_hWND, hDC);
			hDC = NULL;
			return TRUE;
		}
		return FALSE;
	}
}
