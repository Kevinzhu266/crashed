#include <chrono>
#include <format>
#include <iostream>
#include <thread>

#include "core/hooks/hooks.h"
#include <Windows.h>

DWORD WINAPI run_crashed(LPVOID instance) {
    const auto injection_time = std::chrono::high_resolution_clock::now();

    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    constexpr auto game_info_sig = "48 8B 05 ? ? ? ? F2 0F 10 4F ? F2 0F 5A C9 F2 0F 10 17";
    constexpr auto player_list_sig = "48 8B 15 ? ? ? ? 0F B7 59 08 48 C1 E0 03 31 FF 90";
    constexpr auto hud_info_sig = "48 8B 05 ? ? ? ? 0F B6 88 ? ? ? ? 69 C9 ? ? ? ? 89 0D ? ? ? ? 66 89 0D ? ? ? ?";
    //constexpr auto mouse_info_sig = "48 8B 0D ? ? ? ? 48 89 88 ? ? ? ? 89 9C 24 ? ? ? ?";
    constexpr auto view_matrix_sig = "4C 8D 0D ? ? ? ? 48 89 D9 48 89 FA 41 B8";

    context::game_info = *memory::get_offset<GameInfo**>(memory::scan(game_info_sig), 0x3);
    context::player_list = memory::get_offset<PlayerList*>(memory::scan(player_list_sig), 0x3);
    context::hud_info = *memory::get_offset<HudInfo**>(memory::scan(hud_info_sig), 0x3);
    //context::mouse_info = *memory::get_offset<MouseInfo**>(memory::scan(mouse_info_sig), 0x3);
    context::view_matrix = memory::get_offset<ViewMatrix*>(memory::scan(view_matrix_sig), 0x3);

    std::cout << "[addresses]:\n";
    std::cout << std::format("- context::game_info {:#x}\n", std::uintptr_t(context::game_info));
    std::cout << std::format("- context::player_list {:#x}\n", std::uintptr_t(context::player_list));
    std::cout << std::format("- context::hud_info {:#x}\n", std::uintptr_t(context::hud_info));
    //std::cout << std::format("- context::mouse_info {:#x}\n\n", std::uintptr_t(context::mouse_info));
    std::cout << std::format("- context::view_matrix {:#x}\n", std::uintptr_t(context::view_matrix));

    // get the base address of aces.exe
    const auto base = std::uintptr_t(GetModuleHandle(NULL));

    std::cout << "[offsets]:\n";
    std::cout << std::format("- context::game_info {:#x}\n", std::uintptr_t(memory::get_offset<GameInfo*>(memory::scan(game_info_sig), 0x3)) - base);
    std::cout << std::format("- context::player_list {:#x}\n", std::uintptr_t(memory::get_offset<PlayerList*>(memory::scan(player_list_sig), 0x3)) - base);
    std::cout << std::format("- context::hud_info {:#x}\n", std::uintptr_t(memory::get_offset<HudInfo*>(memory::scan(hud_info_sig), 0x3)) - base);
    //std::cout << std::format("- context::mouse_info {:#x}\n\n", std::uintptr_t(memory::get_offset<MouseInfo*>(memory::scan(mouse_info_sig), 0x3)) - base);
    std::cout << std::format("- context::view_matrix {:#x}\n\n", std::uintptr_t(memory::get_offset<ViewMatrix*>(memory::scan(view_matrix_sig), 0x3)) - base);

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
        MessageBox(nullptr, "an unknown  exception occurred", "crashed", MB_ICONERROR);
        hooks::destroy();
        menu::destroy_menu();
        FreeConsole();
        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
    }

    std::cout << "Do not close this window!\nINSERT to open the menu and END to unload the cheat (only loadlib).\n";

    // wait until the unload key is pressed
    while (!GetAsyncKeyState(VK_END) & 1) {
        Sleep(200);
    }

    hooks::destroy();
    menu::destroy_menu();

    std::cout << "crashed has been unloaded. You can safely close this window.\n";

    FreeConsole();
    FreeLibraryAndExitThread(static_cast<HMODULE>(instance), 0);
}

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);

        if (const HANDLE h = CreateThread(nullptr, 0, run_crashed, instance, 0, nullptr); h) {
            CloseHandle(h);
        }
    }

    return TRUE;
}

