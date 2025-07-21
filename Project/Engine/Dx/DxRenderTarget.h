#pragma once
#include "Vector4.h"
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

class RtvManager;
class SrvManager;
class DxCommand;
class DxSwapChain;

class DxRenderTarget {
public:
    DxRenderTarget()  = default;
    ~DxRenderTarget() = default;

    // 初期化
    void Init(Microsoft::WRL::ComPtr<ID3D12Device> device, RtvManager* rtvManager, SrvManager* srvManager,
        DxCommand* dxCommand, DxSwapChain* dxSwapChain, uint32_t width, uint32_t height);

    // 深度バッファ作成
    void CreateDepthBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device);
    void PreRenderTexture();
    void SetupViewportAndScissor();
    void PreDraw();
    void ClearDepthBuffer();
    void PutTransitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After);

    // SRVハンドル作成
    void CreateSrvHandle();

    // リソース状態管理
    void SetRenderTextureState(D3D12_RESOURCE_STATES state) { renderTextureCurrentState_ = state; }
    D3D12_RESOURCE_STATES GetRenderTextureState() const { return renderTextureCurrentState_; }

    // 終了処理
    void Finalize();

private:
    RtvManager* rtvManager_   = nullptr;
    SrvManager* srvManager_   = nullptr;
    DxCommand* dxCommand_     = nullptr;
    DxSwapChain* dxSwapChain_ = nullptr;

    // レンダーテクスチャ作成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device,
        uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);

    // 深度ステンシルテクスチャ作成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device,
        int32_t width, int32_t height);

    // ディスクリプタヒープ初期化
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> InitializeDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device,
        D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

    Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

    D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUSrvHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE renderTextureCPUSrvHandle_;
    D3D12_CLEAR_VALUE clearValue_;
    D3D12_RESOURCE_STATES renderTextureCurrentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

    // ビューポート・シザー矩形
    D3D12_VIEWPORT viewport_{};
    D3D12_RECT scissorRect_{};

    // backBuffer
    uint32_t backBufferWidth_;
    uint32_t backBufferHeight_;

    Vector4 clearColor_;
    UINT descriptorSizeDSV_;

public:
    Microsoft::WRL::ComPtr<ID3D12Resource> GetRenderTextureResource() const { return renderTextureResource_; }
    Microsoft::WRL::ComPtr<ID3D12Resource> GetDepthStencilResource() const { return depthStencilResource_; }
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() const { return dsvDescriptorHeap_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderTextureGPUSrvHandle() const { return renderTextureGPUSrvHandle_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTextureCPUSrvHandle() const { return renderTextureCPUSrvHandle_; }
    D3D12_CLEAR_VALUE GetClearValue() const { return clearValue_; }
    UINT GetDescriptorSizeDSV() const { return descriptorSizeDSV_; }
};