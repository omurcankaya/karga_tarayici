#include "KargaTarayici/UI/RenderEngineD3D11.h"

namespace KargaTarayici::UI {

RenderEngineD3D11::~RenderEngineD3D11() {
    Shutdown();
}

bool RenderEngineD3D11::Initialize(HWND hwnd) {
    hwnd_ = hwnd;

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd_;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, 
        featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &swapChain_, 
        &device_, &featureLevel, &deviceContext_
    );

    if (FAILED(hr)) {
        return false;
    }

    CreateRenderTarget();
    return true;
}

void RenderEngineD3D11::CreateRenderTarget() {
    ID3D11Texture2D* backBuffer{nullptr};
    if (swapChain_ != nullptr) {
        swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (backBuffer != nullptr) {
            device_->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView_);
            backBuffer->Release();
        }
    }
}

void RenderEngineD3D11::CleanupRenderTarget() {
    if (renderTargetView_ != nullptr) {
        renderTargetView_->Release();
        renderTargetView_ = nullptr;
    }
}

void RenderEngineD3D11::ResizeBuffers(UINT width, UINT height) {
    if (device_ != nullptr && swapChain_ != nullptr) {
        CleanupRenderTarget();
        swapChain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        CreateRenderTarget();
    }
}

void RenderEngineD3D11::BeginFrame() {
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (deviceContext_ != nullptr && renderTargetView_ != nullptr) {
        deviceContext_->OMSetRenderTargets(1, &renderTargetView_, nullptr);
        deviceContext_->ClearRenderTargetView(renderTargetView_, clearColor);
    }
}

void RenderEngineD3D11::EndFrame() {
    if (swapChain_ != nullptr) {
        swapChain_->Present(1, 0);
    }
}

void RenderEngineD3D11::Shutdown() {
    CleanupRenderTarget();
    if (swapChain_ != nullptr) {
        swapChain_->Release();
        swapChain_ = nullptr;
    }
    if (deviceContext_ != nullptr) {
        deviceContext_->Release();
        deviceContext_ = nullptr;
    }
    if (device_ != nullptr) {
        device_->Release();
        device_ = nullptr;
    }
}

}
