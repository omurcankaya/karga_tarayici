#include "KargaTarayici/UI/RenderEngineD3D11.h"
#include "KargaTarayici/UI/UIContext.h"
#include "KargaTarayici/Core/IMemoryReader.h"
#include "KargaTarayici/Engine/ScanPipeline.h"
#include "KargaTarayici/Rules/JsonRuleParser.h"
#include <windows.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace {

LRESULT CALLBACK AppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

int main(int argc, char* argv[]) {
    static_cast<void>(argc);
    static_cast<void>(argv);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = AppWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"KargaAppClass";

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0, L"KargaAppClass", L"Karga Address Scanner NextGen",
        WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
    );

    if (hwnd == nullptr) {
        return 1;
    }

    KargaTarayici::UI::RenderEngineD3D11 renderEngine;
    if (!renderEngine.Initialize(hwnd)) {
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(renderEngine.GetDevice(), renderEngine.GetDeviceContext());

    KargaTarayici::UI::UIContext uiContext;

    MSG msg{};
    bool running = true;
    while (running) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (!running) {
            break;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        uiContext.RenderAllPanels();

        ImGui::Render();
        renderEngine.BeginFrame();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        renderEngine.EndFrame();

        Sleep(10);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    renderEngine.Shutdown();
    DestroyWindow(hwnd);
    UnregisterClassW(L"KargaAppClass", GetModuleHandleW(nullptr));

    return 0;
}
