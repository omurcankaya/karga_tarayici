#pragma once

#include "KargaTarayici/Core/Types.h"
#include "KargaTarayici/UI/RenderEngineD3D11.h"
#include "KargaTarayici/UI/UIContext.h"
#include <windows.h>

namespace KargaTarayici::UI {

class OverlayWindow {
private:
    HWND hwnd_{nullptr};
    HWND targetHwnd_{nullptr};
    bool running_{false};
    RenderEngineD3D11 renderEngine_{};
    UIContext uiContext_{};

public:
    OverlayWindow() = default;
    ~OverlayWindow();

    bool Create(HWND targetHwnd);
    void RunLoop();
    void Destroy();

    [[nodiscard]] bool IsRunning() const noexcept { return running_; }
    [[nodiscard]] UIContext& GetUIContext() noexcept { return uiContext_; }
    [[nodiscard]] RenderEngineD3D11& GetRenderEngine() noexcept { return renderEngine_; }
};

}
