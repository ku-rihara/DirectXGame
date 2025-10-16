#pragma once

/// base
#include "Dx/DirectXCommon.h"
#include <vector>

/// SRV管理
class SrvManager {
public:
    SrvManager()  = default;
    ~SrvManager() = default;

    static SrvManager* GetInstance();

    ///===================================================================
    /// public method
    ///===================================================================
    void Init(DirectXCommon* dxCommon);

    // アロケータ
    uint32_t Allocate();

    // 描画前処理
    void PreDraw();

    bool IsAbleSecure();

    /// GPU、CPUデスクリプタハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const uint32_t& index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const uint32_t& index);

    /// SRV生成(テクスチャ用、Structured Buffer用)
    void CreateForTexture2D(const uint32_t& srvIndex, ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);
    void CreateForStructuredBuffer(const uint32_t& srvIndex, ID3D12Resource* pResource, const UINT& numElements, const UINT& structuredByteStride);

    void CreateStructuredUAV(const uint32_t& index, ID3D12Resource* resource, const UINT& numElements, const UINT& byteStride);

private:
    ///=========================================
    /// private variant
    ///=========================================

    /// other class
    DirectXCommon* dxCommon_ = nullptr;

    uint32_t descriptorSize_; //< デスクリプタサイズ
    uint32_t useIndex_ = 0; //< 次に使用するインデックス
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

    // リソースとデータ
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> instancingResources_;

public:
    // 最大SRV数
    static const uint32_t kMaxCount;

    ///===================================================================
    /// getter method
    ///===================================================================

    const uint32_t& GetDescriptorSize() const { return descriptorSize_; }
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_.Get(); }

    ///===================================================================
    /// setter method
    ///===================================================================
    void SetDescriptorHeap(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& srv) { descriptorHeap_ = srv; }
};