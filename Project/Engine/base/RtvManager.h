#pragma once

/// base
#include "Dx/DirectXCommon.h"

/// SRV管理
class RtvManager {
public:
    RtvManager()  = default;
    ~RtvManager() = default;

    static RtvManager* GetInstance();

    ///===================================================================
    /// public method
    ///===================================================================
    void Init(DirectXCommon* directXCommon);

    // アロケータ
    uint32_t Allocate();

    bool IsAbleSecure();

    /// GPU、CPUデスクリプタハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

    // RTV作成
    void CreateRTV(uint32_t index, ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc);

private:
    ///=========================================
    /// private variant
    ///=========================================

    /// other class
    DirectXCommon* dxCommon_ = nullptr;

    uint32_t descriptorSize_; // RTV用のデスクリプタサイズ
    uint32_t useIndex_ = 0; // 次に使用するインデックス

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_; // RTV用デスクリプタ

public:
    // 最大SRV数
    static const uint32_t kMaxCount;

public:
    ///===================================================================
    /// getter method
    ///===================================================================

    uint32_t GetDescriptorSizeSRV() const { return descriptorSize_; }
    ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return descriptorHeap_.Get(); }

    ///===================================================================
    /// setter method
    ///===================================================================
    void SetSrvDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srv) { this->descriptorHeap_ = srv; }
};