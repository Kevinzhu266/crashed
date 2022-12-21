#pragma once
#include <d3d11.h>

#include "hacks/hacks.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

namespace menu
{
	inline bool open = true;
	void render() noexcept;

	void create_menu(IDXGISwapChain* swap_chain) noexcept;
	void destroy_menu() noexcept;

	inline HWND window = nullptr;
	inline WNDPROC original_window_process = nullptr;

	void create();
	inline std::uint64_t* method_table = nullptr;
}
