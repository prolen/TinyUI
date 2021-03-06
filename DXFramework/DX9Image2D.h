#pragma once
#include "DX9Element2D.h"
#include "DX9Texture2D.h"
#include "Render/TinyGDI.h"
using namespace TinyUI;

namespace DXFramework
{
	class DX9Image2D : public DX9Element2D, public DX9Texture2D
	{
		struct VERTEXTYPE
		{
			FLOAT x, y, z, rhw;
			FLOAT u, v;
		};
		DECLARE_DYNAMIC(DX9Image2D)
		DISALLOW_COPY_AND_ASSIGN(DX9Image2D)
	public:
		DX9Image2D();
		virtual ~DX9Image2D();
		BOOL Translate(DX9& dx8, FLOAT ratioX = 1.0F, FLOAT ratioY = 1.0F);
		BOOL Load(DX9& dx9, const CHAR* pzFile);
		BOOL Load(DX9& dx9, const BYTE* bits, LONG size);
		BOOL Create(DX9& dx9, INT cx, INT cy, const BYTE* bits) OVERRIDE;
	public:
		virtual INT	GetVertexCount() const;
	public:
		BOOL Allocate(DX9& dx9) OVERRIDE;
		BOOL Process(DX9& dx9) OVERRIDE;
		void Deallocate(DX9& dx9) OVERRIDE;
	private:
		BOOL Initialize(DX9& dx9);
	protected:
		TinyComPtr<IDirect3DVertexBuffer9>	m_vertexBuffer;
	};
}

