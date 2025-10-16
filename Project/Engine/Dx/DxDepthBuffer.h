#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl/client.h>

class DsvManager;
class SrvManager;

class DxDepthBuffer {
public:
    DxDepthBuffer()  = default;
    ~DxDepthBuffer() = default;

    // 初期化
    void Init(Microsoft::WRL::ComPtr<ID3D12Device> device, DsvManager* dsvManager,
        SrvManager* srvManager, const uint32_t& width, const uint32_t& height);

    // 深度バッファクリア、リソース状態遷移
    void Clear(ID3D12GraphicsCommandList* commandList);
    void TransitionState(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& newState);

    // 終了処理
    void Finalize();

private:
    // 深度ステンシルリソース作成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilResource(
        Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height);

    // DSV作成
    void CreateDSV();

    // 深度テクスチャ用SRV作成
    void CreateDepthSRV();

    DsvManager* dsvManager_ = nullptr;
    SrvManager* srvManager_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

    D3D12_GPU_DESCRIPTOR_HANDLE depthTextureGPUSrvHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE depthTextureCPUSrvHandle_;

    D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_DEPTH_WRITE;

    uint32_t depthStencilDsvIndex_ = 0;
    uint32_t depthTextureSrvIndex_ = 0;

    uint32_t width_  = 0;
    uint32_t height_ = 0;

public:
    // ゲッター
    ID3D12Resource* GetDepthStencilResource() const { return depthStencilResource_.Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const;
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetDepthSrvGPUHandle() const { return depthTextureGPUSrvHandle_; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetDepthSrvCPUHandle() const { return depthTextureCPUSrvHandle_; }
    const D3D12_RESOURCE_STATES& GetCurrentState() const { return currentState_; }
};