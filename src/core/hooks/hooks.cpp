#include "hooks.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <format>
#include <filesystem>

#include <minhook/minhook.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include "../context.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

BOOL KeVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect) {
	return NT_SUCCESS(_syscall<NTSTATUS>(0x50,
		((HANDLE)-1),
		&lpAddress,
		&dwSize,
		PAGE_EXECUTE_READWRITE,
		lpflOldProtect)) ? TRUE : FALSE;
}

void hooks::create() {
	if (MH_Initialize()) {
		throw std::runtime_error("Failed to initialize minhook");
	}

	if (MH_CreateHook(reinterpret_cast<void*>(menu::method_table[8]), &present, reinterpret_cast<void**>(&original_present_func))) {
		throw std::runtime_error("Unable to hook present");
	}

	if (MH_EnableHook(reinterpret_cast<void*>(menu::method_table[8]))) {
		throw std::runtime_error("Unable to enable present");
	}
}

void hooks::destroy() noexcept {
	MH_DisableHook(reinterpret_cast<void*>(menu::method_table[8]));
	MH_RemoveHook(reinterpret_cast<void*>(menu::method_table[8]));
	MH_Uninitialize();
}

HRESULT __stdcall hooks::present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags) noexcept {
	if (static auto menu_setup = false; !menu_setup) {
		menu::create_menu(swap_chain);
		menu_setup = !menu_setup;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (menu::open) {
		menu::render();
	}

	hacks::visuals();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return original_present_func(swap_chain, sync_interval, flags);
}
