#pragma once
#include "../menu.h"

namespace hooks {
	void create();
	void destroy() noexcept;
}

namespace hooks {
	using PresentFunc = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT) noexcept;
	inline PresentFunc original_present_func{ nullptr };
	HRESULT __stdcall present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) noexcept;
}
