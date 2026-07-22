#include "KargaTarayici/UI/OverlayWindow.h"
#include "KargaTarayici/Utils/Logger.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace KargaTarayici::UI {

namespace {

OverlayWindow* g_overlayInstance = nullptr;

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }

    switch (msg) {
    case WM_SIZE:
        if (g_overlayInstance != nullptr && wParam != SIZE_MINIMIZED) {
            UINT width = static_cast<UINT>(LOWORD(lParam));
            UINT height = static_cast<UINT>(HIWORD(lParam));
            g_overlayInstance->GetRenderEngine().ResizeBuffers(width, height);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

}

OverlayWindow::~OverlayWindow() {
    Destroy();
}

bool OverlayWindow::Create(HWND targetHwnd) {
    targetHwnd_ = targetHwnd;
    g_overlayInstance = this;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"KargaWindowInterfaceClass";

    RegisterClassExW(&wc);

    int width = 1100;
    int height = 700;

    hwnd_ = CreateWindowExW(
        0,
        L"KargaWindowInterfaceClass",
        L"Karga Scanner Interface [Injected Window]",
        WS_OVERLAPPEDWINDOW,
        150, 150, width, height,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
    );

    if (hwnd_ == nullptr) {
        return false;
    }

    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);

    if (!renderEngine_.Initialize(hwnd_)) {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd_);
    ImGui_ImplDX11_Init(renderEngine_.GetDevice(), renderEngine_.GetDeviceContext());

    running_ = true;
    return true;
}

void OverlayWindow::RunLoop() {
    MSG msg{};
    while (running_) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running_ = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (!running_) {
            break;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        uiContext_.RenderAllPanels();

        ImGui::Render();
        renderEngine_.BeginFrame();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        renderEngine_.EndFrame();

        Sleep(10);
    }
}

void OverlayWindow::Destroy() {
    if (running_) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        renderEngine_.Shutdown();

        if (hwnd_ != nullptr) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
        UnregisterClassW(L"KargaWindowInterfaceClass", GetModuleHandleW(nullptr));
        g_overlayInstance = nullptr;
        running_ = false;
    }
}

}
