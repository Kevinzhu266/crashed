#include <thread>
#include <iostream>
#include <format>
#include <chrono>

#include <Windows.h>
#include "hooks/hooks.h"

DWORD WINAPI run_crashed(LPVOID instance) {
	const auto injection_time = std::chrono::high_resolution_clock::now();

	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	constexpr auto game_info_sig = "48 8B 0D ? ? ? ? 48 85 C9 0F 84 ? ? ? ? C6 05 ? ? ? ? ?";
	constexpr auto player_list_sig = "48 8B 35 ? ? ? ? 4C 8D 3C C6";

	context::game_info = *memory::get_offset<GameInfo**>(memory::scan(game_info_sig), 0x3);
	context::player_list = memory::get_offset<PlayerList*>(memory::scan(player_list_sig), 0x3);

	std::cout << "[addresses]:\n";
	std::cout << std::format("- context::game_info {:#x}\n", std::uintptr_t(context::game_info));
	std::cout << std::format("- context::player_list {:#x}\n\n", std::uintptr_t(context::player_list));

	const auto base = std::uintptr_t(GetModuleHandle(NULL));

	std::cout << "[offsets]:\n";
	std::cout << std::format("- context::game_info {:#x}\n", std::uintptr_t(memory::get_offset<GameInfo*>(memory::scan(game_info_sig), 0x3)) - base);
	std::cout << std::format("- context::player_list {:#x}\n\n", std::uintptr_t(memory::get_offset<PlayerList*>(memory::scan(player_list_sig), 0x3)) - base);

	try {
		menu::create();
		hooks::create();
	}
	catch (const std::exception& error) {
		MessageBox(nullptr, error.what(), "crashed", MB_ICONERROR);
		hooks::destroy();
		menu::destroy_menu();
		FreeConsole();
		FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
	}
	catch (...) {
		MessageBox(nullptr, "an unknown  exception occured", "crashed", MB_ICONERROR);
		hooks::destroy();
		menu::destroy_menu();
		FreeConsole();
		FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
	}

	// wait until the unload key is pressed
	while (!GetAsyncKeyState(VK_END)) {
		Sleep(200);
	}

	hooks::destroy();
	menu::destroy_menu();

	FreeConsole();
	FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(instance);

		HANDLE thread = CreateThread(nullptr, 0, run_crashed, instance, 0, nullptr);

		if (thread) {
			CloseHandle(thread);
		}
	}

	return TRUE;
}

