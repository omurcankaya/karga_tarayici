#include "KargaTarayici/UI/OverlayWindow.h"
#include <windows.h>

namespace {

DWORD WINAPI OverlayThread(LPVOID param) {
    auto targetHwnd = static_cast<HWND>(param);
    KargaTarayici::UI::OverlayWindow overlay;
    if (overlay.Create(targetHwnd)) {
        overlay.RunLoop();
    }
    return 0;
}

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    static_cast<void>(hModule);
    static_cast<void>(lpReserved);

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        QueueUserWorkItem(OverlayThread, GetForegroundWindow(), WT_EXECUTELONGFUNCTION);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
