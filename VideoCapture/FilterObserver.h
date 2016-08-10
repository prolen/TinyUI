#pragma once
#include <Windows.h>

namespace Media
{
	class FilterObserver
	{
	public:
		virtual void OnFrameReceive(const BYTE* data, INT size) = 0;
	protected:
		virtual ~FilterObserver();
	};
}
