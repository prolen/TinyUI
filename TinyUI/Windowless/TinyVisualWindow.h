#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualWindow : public TinyVisual
		{
			friend class TinyVisualBuilder;
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualWindow)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualWindow)
		protected:
			TinyVisualWindow();
			TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualWindow();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
			void OnSizeChange(const TinySize&, const TinySize&) OVERRIDE;
			void SetPosition(const TinyPoint& pos) OVERRIDE;
			void SetText(const TinyString& pzText) OVERRIDE;
		public:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
	}
}



