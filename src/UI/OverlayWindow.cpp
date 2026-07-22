#include "KargaTarayici/UI/OverlayWindow.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace KargaTarayici::UI {

namespace {

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }

    switch (msg) {
    case WM_SIZE:
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

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"KargaOverlayClass";

    RegisterClassExW(&wc);

    RECT targetRect{0, 0, 1280, 720};
    if (targetHwnd_ != nullptr && IsWindow(targetHwnd_)) {
        GetWindowRect(targetHwnd_, &targetRect);
    }

    int width = targetRect.right - targetRect.left;
    int height = targetRect.bottom - targetRect.top;

    hwnd_ = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        L"KargaOverlayClass",
        L"Karga Scanner Overlay",
        WS_POPUP,
        targetRect.left, targetRect.top, width, height,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
    );

    if (hwnd_ == nullptr) {
        return false;
    }

    SetLayeredWindowAttributes(hwnd_, RGB(0, 0, 0), 255, LWA_ALPHA);
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

        if (targetHwnd_ != nullptr && IsWindow(targetHwnd_)) {
            RECT rect{};
            GetWindowRect(targetHwnd_, &rect);
            SetWindowPos(hwnd_, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE);
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
        UnregisterClassW(L"KargaOverlayClass", GetModuleHandleW(nullptr));
        running_ = false;
    }
}

}
