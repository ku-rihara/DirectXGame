#pragma once
#include "Vector4.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <cstdint>

class RtvManager;
class SrvManager;

class DxRenderTarget {
public:

    DxRenderTarget()  = default;
    ~DxRenderTarget() = default;

    // 初期化
    void Init(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        RtvManager* rtvManager,
        SrvManager* srvManager,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format,
        const Vector4& clearColor);

    // 深度バッファ作成
    void CreateDepthBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height);
    void PreRenderTexture();

    // SRVハンドル作成
    void CreateSrvHandle();

    // ゲッター
    Microsoft::WRL::ComPtr<ID3D12Resource> GetRenderTextureResource() const { return renderTextureResource_; }
    Microsoft::WRL::ComPtr<ID3D12Resource> GetDepthStencilResource() const { return depthStencilResource_; }
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() const { return dsvDescriptorHeap_; }
    D3D12_GPU_DESCRIPTOR_HANDLE GetRenderTextureGPUSrvHandle() const { return renderTextureGPUSrvHandle_; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTextureCPUSrvHandle() const { return renderTextureCPUSrvHandle_; }
    D3D12_CLEAR_VALUE GetClearValue() const { return clearValue_; }
    UINT GetDescriptorSizeDSV() const { return descriptorSizeDSV_; }

    // リソース状態管理
    void SetRenderTextureState(D3D12_RESOURCE_STATES state) { renderTextureCurrentState_ = state; }
    D3D12_RESOURCE_STATES GetRenderTextureState() const { return renderTextureCurrentState_; }

    // 終了処理
    void Finalize();

private:
    // レンダーテクスチャ作成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format,
        const Vector4& clearColor);

    // 深度ステンシルテクスチャ作成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        int32_t width,
        int32_t height);

    // ディスクリプタヒープ初期化
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> InitializeDescriptorHeap(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        UINT numDescriptors,
        bool shaderVisible);

    Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

    D3D12_GPU_DESCRIPTOR_HANDLE renderTextureGPUSrvHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE renderTextureCPUSrvHandle_;
    D3D12_CLEAR_VALUE clearValue_;
    D3D12_RESOURCE_STATES renderTextureCurrentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

    UINT descriptorSizeDSV_;
    RtvManager* rtvManager_ = nullptr;
    SrvManager* srvManager_ = nullptr;
};