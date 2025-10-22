#pragma once

/// base
#include "Dx/DirectXCommon.h"

/// <summary>
/// DSV管理クラス
/// </summary>
class DsvManager {
public:
    DsvManager()  = default;
    ~DsvManager() = default;

    /// <summary>
    /// シングルトンインスタンス取得
    /// </summary>
    /// <returns>DsvManagerのインスタンス</returns>
    static DsvManager* GetInstance();

    ///===================================================================
    /// public method
    ///===================================================================

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="directXCommon">DirectXCommon</param>
    void Init(DirectXCommon* directXCommon);

    /// <summary>
    /// DSVインデックス割り当て
    /// </summary>
    /// <returns>割り当てられたインデックス</returns>
    uint32_t Allocate();

    /// <summary>
    /// 確保可能かチェック
    /// </summary>
    /// <returns>確保可能ならtrue</returns>
    bool IsAbleSecure();

    /// <summary>
    /// CPUデスクリプタハンドル取得
    /// </summary>
    /// <param name="index">インデックス</param>
    /// <returns>CPUデスクリプタハンドル</returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const uint32_t& index);

    /// <summary>
    /// DSV作成
    /// </summary>
    /// <param name="index">インデックス</param>
    /// <param name="resource">リソース</param>
    /// <param name="desc">DSVデスク</param>
    void Create(const uint32_t& index, ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC* desc);

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