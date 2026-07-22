#pragma once

#include "KargaTarayici/Core/Types.h"
#include <windows.h>
#include <d3d11.h>

namespace KargaTarayici::UI {

class RenderEngineD3D11 {
private:
    ID3D11Device* device_{nullptr};
    ID3D11DeviceContext* deviceContext_{nullptr};
    IDXGISwapChain* swapChain_{nullptr};
    ID3D11RenderTargetView* renderTargetView_{nullptr};
    HWND hwnd_{nullptr};

public:
    RenderEngineD3D11() = default;
    ~RenderEngineD3D11();

    bool Initialize(HWND hwnd);
    void Shutdown();
    void ResizeBuffers(UINT width, UINT height);
    void BeginFrame();
    void EndFrame();

    void CreateRenderTarget();
    void CleanupRenderTarget();

    [[nodiscard]] ID3D11Device* GetDevice() const noexcept { return device_; }
    [[nodiscard]] ID3D11DeviceContext* GetDeviceContext() const noexcept { return deviceContext_; }
};

}
