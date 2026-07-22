#pragma once

#include "KargaTarayici/Core/Types.h"
#include <windows.h>

namespace KargaTarayici::UI {

class OverlayWindow {
private:
    HWND hwnd_{nullptr};
    HWND targetHwnd_{nullptr};
    bool running_{false};

public:
    OverlayWindow() = default;
    ~OverlayWindow();

    bool Create(HWND targetHwnd);
    void RunLoop();
    void Destroy();

    [[nodiscard]] bool IsRunning() const noexcept { return running_; }
};

}
