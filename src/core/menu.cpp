#include "menu.h"

#include <fstream>
#include <stdexcept>

#include "../sdk/players.h"

LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);

void menu::render() noexcept {
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin(
        "crashed",
        nullptr,
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
        ImGui::Text("misc");
        ImGui::Separator();
        ImGui::Checkbox("watermark", &hacks::config::watermark);

        ImGui::Spacing();

        ImGui::Text("esp");
        ImGui::Separator();
        ImGui::Checkbox("enabled", &hacks::config::esp_enabled);
        ImGui::Checkbox("enemy only", &hacks::config::esp_enemy_only);
        ImGui::ColorPicker4("main color", hacks::config::esp_main_colour.data(), ImGuiColorEditFlags_NoPicker);
        ImGui::ColorPicker4("front color", hacks::config::esp_front_colour.data());

        ImGui::Spacing();

        ImGui::Text("TODO: Add menu items here...");
        ImGui::End();
    }
}

void menu::create_menu(IDXGISwapChain* swap_chain) noexcept {
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    ID3D11RenderTargetView* mainRenderTargetView;

    if (SUCCEEDED(swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device)))) {
        device->GetImmediateContext(&context);

        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        swap_chain->GetDesc(&swapChainDesc);
        window = swapChainDesc.OutputWindow;

        ID3D11Texture2D* backBuffer;
        swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBuffer));
        device->CreateRenderTargetView(backBuffer, NULL, &mainRenderTargetView);
        backBuffer->Release();

        original_window_process = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window,
            GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(WindowProcedure)));

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

        //io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahoma.ttf", 16.f);
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 15.f);

        ImGuiStyle* style = &ImGui::GetStyle();

        style->WindowPadding = {15, 15};
        style->WindowRounding = 1.f;
        style->WindowTitleAlign = {0.5f, 0.5f};
        style->WindowBorderSize = 0;

        style->FrameRounding = 0.f;
        style->FramePadding = {20, 7};
        style->FrameBorderSize = 1;

        assert(device);
        assert(window);

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(device, context);

        ImGui::SetColorEditOptions(ImGuiColorEditFlags_NoInputs
            | ImGuiColorEditFlags_NoLabel
            | ImGuiColorEditFlags_NoSidePreview
            | ImGuiColorEditFlags_Uint8
            | ImGuiColorEditFlags_AlphaBar);
    }
}

void menu::destroy_menu() noexcept {
    free(method_table);

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    SetWindowLongPtrW(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_window_process));
}

void menu::create() {
    WNDCLASSEX windowClass;
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = DefWindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hIcon = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = "Crashed";
    windowClass.hIconSm = NULL;

    if (!RegisterClassEx(&windowClass)) {
        throw std::runtime_error("Failed to register window class.");
    }

    const auto tempWindow = CreateWindow(windowClass.lpszClassName,
        "Crashed DirectX Window",
        WS_OVERLAPPEDWINDOW,
        0, 0,
        100, 100,
        NULL,
        NULL,
        windowClass.hInstance,
        NULL);

    if (!tempWindow) {
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
        throw std::runtime_error("Failed to create temp window.");
    }

    const auto d3dlib = GetModuleHandleA("d3d11.dll");

    if (!d3dlib) {
        DestroyWindow(tempWindow);
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
        throw std::runtime_error("Unable to get d3d11.dll handle");
    }

    using Fn = HRESULT(__stdcall*)(
        IDXGIAdapter*,
        D3D_DRIVER_TYPE,
        HMODULE,
        UINT,
        const D3D_FEATURE_LEVEL*,
        UINT,
        UINT,
        const DXGI_SWAP_CHAIN_DESC*,
        IDXGISwapChain**,
        ID3D11Device**,
        D3D_FEATURE_LEVEL*,
        ID3D11DeviceContext**);

    const auto function = reinterpret_cast<Fn>(GetProcAddress(d3dlib, "D3D11CreateDeviceAndSwapChain"));

    if (!function) {
        DestroyWindow(tempWindow);
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
        throw std::runtime_error("Unable to get D3D11CreateDeviceAndSwapChain");
    }

    D3D_FEATURE_LEVEL featureLevel = { };
    const D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0};

    DXGI_RATIONAL refreshRate;
    refreshRate.Numerator = 60;
    refreshRate.Denominator = 1;

    DXGI_MODE_DESC bufferDesc;
    bufferDesc.Width = 100;
    bufferDesc.Height = 100;
    bufferDesc.RefreshRate = refreshRate;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc = sampleDesc;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = tempWindow;
    swapChainDesc.Windowed = 1;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    IDXGISwapChain* swapChain;
    ID3D11Device* device;
    ID3D11DeviceContext* context;

    if (function(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        featureLevels,
        2,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &swapChain, &device,
        &featureLevel,
        &context) < 0)
    {
        DestroyWindow(tempWindow);
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
        throw std::runtime_error("Fuck knows what this problem is");
    }

    method_table = reinterpret_cast<uint64_t*>(calloc(205, sizeof(uint64_t)));
    memcpy(method_table, *reinterpret_cast<uint64_t**>(swapChain), 18 * sizeof(uint64_t));
    memcpy(method_table + 18, *reinterpret_cast<uint64_t**>(device), 43 * sizeof(uint64_t));
    memcpy(method_table + 18 + 43, *reinterpret_cast<uint64_t**>(context), 144 * sizeof(uint64_t));

    swapChain->Release();
    swapChain = NULL;

    device->Release();
    device = NULL;

    context->Release();
    context = NULL;

    DestroyWindow(tempWindow);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParam, LPARAM longParam);
LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wideParam, LPARAM longParam) {
    if (GetAsyncKeyState(VK_INSERT) & 1)
        menu::open = !menu::open;

    if (menu::open && ImGui_ImplWin32_WndProcHandler(window, message, wideParam, longParam))
        return 1L;

    return CallWindowProcW(menu::original_window_process, window, message, wideParam, longParam);
}
