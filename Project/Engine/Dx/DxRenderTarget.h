#pragma once
#include "DxDepthBuffer.h"
#include "Vector4.h"
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include <wrl/client.h>

class RtvManager;
class SrvManager;
class DxCommand;
class DxSwapChain;
class DxDepthBuffer;
class DxRenderTarget {
public:
    DxRenderTarget()  = default;
    ~DxRenderTarget() = default;

    // 初期化
    void Init(Microsoft::WRL::ComPtr<ID3D12Device> device, DxDepthBuffer* depthBuffer, RtvManager* rtvManager, SrvManager* srvManager,
        DxCommand* dxCommand, DxSwapChain* dxSwapChain, const uint32_t& width, const uint32_t& height);

    void PreRenderTexture();
    void PreDraw();
    void PostDrawTransitionBarrier();
    void SetupViewportAndScissor();
    void Finalize();

private:
    void CreateRenderTextureRTV();
    void CreateRenderTextureSRV();
    void PutTransitionBarrier(ID3D12Resource* pResource, const D3D12_RESOURCE_STATES& Before, const D3D12_RESOURCE_STATES& After);

    Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(
        Microsoft::WRL::ComPtr<ID3D12Device> device, const uint32_t& width, const uint32_t& height,
        DXGI_FORMAT format, const Vector4& clearColor);

private:
    RtvManager* rtvManager_   = nullptr;
    SrvManager* srvManager_   = nullptr;
    DxCommand* dxCommand_     = nullptr;
    DxSwapChain* dxSwapChain_ = nullptr;

    DxDepthBuffer* depthBuffer_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
    D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUSrvHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE renderTextureCPUSrvHandle_;
    D3D12_CLEAR_VALUE clearValue_;
    D3D12_RESOURCE_STATES renderTextureCurrentState_;

    UINT backBufferIndex_;
    D3D12_VIEWPORT viewport_{};
    D3D12_RECT scissorRect_{};
    uint32_t backBufferWidth_;
    uint32_t backBufferHeight_;
    Vector4 clearColor_;
    uint32_t renderTextureRtvIndex_;

public:
    // ゲッター
    Microsoft::WRL::ComPtr<ID3D12Resource> GetRenderTextureResource() const { return renderTextureResource_; }
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetRenderTextureGPUSrvHandle() const { return renderTextureGPUSrvHandle_; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetRenderTextureCPUSrvHandle() const { return renderTextureCPUSrvHandle_; }
    const D3D12_CLEAR_VALUE& GetClearValue() const { return clearValue_; }
};