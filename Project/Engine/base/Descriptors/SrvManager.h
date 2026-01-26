#pragma once

/// base
#include "Base/Dx/DirectXCommon.h"
#include <vector>

/// <summary>
/// SRV管理クラス
/// </summary>
namespace KetaEngine {

class SrvManager {
public:
    SrvManager()  = default;
    ~SrvManager() = default;

    /// <summary>
    /// シングルトンインスタンス取得
    /// </summary>
    /// <returns>SrvManagerのインスタンス</returns>
    static SrvManager* GetInstance();

    ///===================================================================
    /// public method
    ///===================================================================

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// SRVインデックス割り当て
    /// </summary>
    /// <returns>割り当てられたインデックス</returns>
    uint32_t Allocate();

    void PreDraw(); //< 描画前処理

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
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

    /// <summary>
    /// GPUデスクリプタハンドル取得
    /// </summary>
    /// <param name="index">インデックス</param>
    /// <returns>GPUデスクリプタハンドル</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

    /// <summary>
    /// テクスチャ用SRV作成
    /// </summary>
    /// <param name="srvIndex">SRVインデックス</param>
    /// <param name="pResource">リソース</param>
    /// <param name="desc">SRVデスク</param>
    void CreateForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

    /// <summary>
    /// StructuredBuffer用SRV作成
    /// </summary>
    /// <param name="srvIndex">SRVインデックス</param>
    /// <param name="pResource">リソース</param>
    /// <param name="numElements">要素数</param>
    /// <param name="structuredByteStride">ストライド</param>
    void CreateForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, const UINT& numElements, const UINT& structuredByteStride);

    /// <summary>
    /// StructuredBuffer用UAV作成
    /// </summary>
    /// <param name="index">インデックス</param>
    /// <param name="resource">リソース</param>
    /// <param name="numElements">要素数</param>
    /// <param name="byteStride">ストライド</param>
    void CreateStructuredUAV(uint32_t index, ID3D12Resource* resource, const UINT& numElements, const UINT& byteStride);

private:
    ///=========================================
    /// private variant
    ///=========================================

    /// other class
    DirectXCommon* dxCommon_ = nullptr;

    uint32_t descriptorSize_; //< デスクリプタサイズ
    uint32_t useIndex_ = 0;   //< 次に使用するインデックス
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
  
    // リソースとデータ
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> instancingResources_;

public:
    // 最大SRV数
    static const uint32_t kMaxCount;

    ///===================================================================
    /// getter method
    ///===================================================================

    uint32_t GetDescriptorSize() const { return descriptorSize_; }
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_.Get(); }


    ///===================================================================
    /// setter method
    ///===================================================================
    void SetDescriptorHeap(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& srv) { descriptorHeap_ = srv; }
};

}; // KetaEngine
