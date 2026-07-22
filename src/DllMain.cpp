#include "KargaTarayici/UI/OverlayWindow.h"
#include "KargaTarayici/Utils/Logger.h"
#include <windows.h>

namespace {

struct EnumWindowData {
    DWORD processId{0};
    HWND hwnd{nullptr};
};

BOOL CALLBACK EnumProcessWindowsProc(HWND hwnd, LPARAM lParam) {
    auto data = reinterpret_cast<EnumWindowData*>(lParam);
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == data->processId && IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == nullptr) {
        data->hwnd = hwnd;
        return FALSE;
    }
    return TRUE;
}

HWND GetCurrentProcessMainWindow() {
    EnumWindowData data{};
    data.processId = GetCurrentProcessId();
    EnumWindows(EnumProcessWindowsProc, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}

DWORD WINAPI OverlayThread(LPVOID param) {
    static_cast<void>(param);

    KargaTarayici::Utils::Logger::Initialize(true, true, "KargaScanner_dll.log");
    KargaTarayici::Utils::Logger::Info("KargaScanner.dll loaded inside target process.");

    Sleep(500);

    HWND targetHwnd = GetCurrentProcessMainWindow();
    if (targetHwnd != nullptr) {
        KargaTarayici::Utils::Logger::Info("Found main window handle for target process.");
    } else {
        KargaTarayici::Utils::Logger::Warning("Main window handle is NULL. Overlay will render on desktop bounds.");
    }

    KargaTarayici::UI::OverlayWindow overlay;
    if (overlay.Create(targetHwnd)) {
        KargaTarayici::Utils::Logger::Info("Overlay Window created. Entering main UI render loop.");
        overlay.RunLoop();
    } else {
        KargaTarayici::Utils::Logger::Error("Failed to create Overlay Window.");
    }

    KargaTarayici::Utils::Logger::Shutdown();
    return 0;
}

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    static_cast<void>(hModule);
    static_cast<void>(lpReserved);

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        QueueUserWorkItem(OverlayThread, nullptr, WT_EXECUTELONGFUNCTION);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
