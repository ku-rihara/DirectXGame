#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl/client.h>

class DsvManager;
class SrvManager;

/// <summary>
/// 深度バッファを管理するクラス
/// </summary>
class DxDepthBuffer {
public:
    DxDepthBuffer()  = default;
    ~DxDepthBuffer() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    /// <param name="dsvManager">DSVマネージャー</param>
    /// <param name="srvManager">SRVマネージャー</param>
    /// <param name="width">バッファ幅</param>
    /// <param name="height">バッファ高さ</param>
    void Init(Microsoft::WRL::ComPtr<ID3D12Device> device, DsvManager* dsvManager,
        SrvManager* srvManager, uint32_t width, uint32_t height);

    /// <summary>
    /// 深度バッファクリア
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void Clear(ID3D12GraphicsCommandList* commandList);

    /// <summary>
    /// リソース状態遷移
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="newState">新しい状態</param>
    void TransitionState(ID3D12GraphicsCommandList* commandList, const D3D12_RESOURCE_STATES& newState);

    //< 終了処理
    void Finalize();

private:
    /// <summary>
    /// 深度ステンシルリソース作成
    /// </summary>
    /// <param name="device">D3D12デバイス</param>
    /// <param name="width">バッファ幅</param>
    /// <param name="height">バッファ高さ</param>
    /// <returns>深度ステンシルリソース</returns>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilResource(
        Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height);
  
    void CreateDSV();//< DSV作成
    void CreateDepthSRV(); //< 深度テクスチャ用SRV作成

private:
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
    ID3D12Resource* GetDepthStencilResource() const { return depthStencilResource_.Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const;
    const D3D12_GPU_DESCRIPTOR_HANDLE& GetDepthSrvGPUHandle() const { return depthTextureGPUSrvHandle_; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetDepthSrvCPUHandle() const { return depthTextureCPUSrvHandle_; }
    const D3D12_RESOURCE_STATES& GetCurrentState() const { return currentState_; }
};