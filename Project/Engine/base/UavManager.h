#pragma once

/// base
#include "Dx/DirectXCommon.h"

class UavManager {
public:
    UavManager()  = default;
    ~UavManager() = default;

    static UavManager* GetInstance();

    ///===================================================================
    /// public method
    ///===================================================================
    void Init(DirectXCommon* directXCommon);

    // アロケータ
    uint32_t Allocate();

    bool IsAbleSecure();

    /// GPU、CPUデスクリプタハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const uint32_t& index);

    // RTV作成
    void Create(const uint32_t& index, ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc);

private:
    ///=========================================
    /// private variant
    ///=========================================

    /// other class
    DirectXCommon* dxCommon_ = nullptr;

    uint32_t descriptorSize_; //< デスクリプタサイズ
    uint32_t useIndex_ = 0; //< 次に使用するインデックス

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

public:
    // 最大数
    static const uint32_t kMaxCount;

public:
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