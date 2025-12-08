#include "SrvManager.h"
#include <cassert>

// 最大SRV数の定義
const uint32_t SrvManager::kMaxCount = 1024*6;

// シングルトンインスタンスの取得
SrvManager* SrvManager::GetInstance() {
    static SrvManager instance;
    return &instance;
}

///=========================================
/// 初期化
///=========================================
void SrvManager::Init(DirectXCommon* dxCommon) {
    this->dxCommon_ = dxCommon;

    // でスクリプタヒープ生成
    descriptorHeap_ = dxCommon_->InitializeDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxCount, true);
    /// デスクリプタ一個分のサイズを取得して記録
    descriptorSize_ = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

///=========================================
/// アロケータ
///=========================================
uint32_t SrvManager::Allocate() {

    assert(IsAbleSecure());

    // return する番号を一旦記録
    int index = useIndex_;

    // 次回の為に番号を1進める
    useIndex_++;

    // 上で記録した番号をreturn
    return index;
}

bool SrvManager::IsAbleSecure() {
    if (useIndex_ >= SrvManager::kMaxCount) {
        return false;
    } else {
        return true;
    }
}

///==========================================
/// 描画前処理
///=========================================
void SrvManager::PreDraw() {
    // コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = {descriptorHeap_.Get()};
    dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());
}

///=========================================
/// GPU、CPUデスクリプタハンドル取得
///=========================================
D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index) {
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize_ * index);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index) {
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize_ * index);
    return handleGPU;
}

///=========================================
/// テクスチャ用SRV生成
///=========================================
void SrvManager::CreateForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc = desc;
    // 生成
    dxCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

///=========================================
/// StructureBuffer用SRV生成
///=========================================
void SrvManager::CreateForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, const UINT& numElements, const UINT& structuredByteStride) {

    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
    instancingSrvDesc.Format                     = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc.Buffer.FirstElement        = 0;
    instancingSrvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc.Buffer.NumElements         = numElements;
    instancingSrvDesc.Buffer.StructureByteStride = structuredByteStride;

    instancingResources_.push_back(pResource);

    dxCommon_->GetDevice()->CreateShaderResourceView(pResource, &instancingSrvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvManager::CreateStructuredUAV(uint32_t index, ID3D12Resource* resource, const UINT& numElements, const UINT& byteStride) {
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format                      = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension               = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement         = 0;
    uavDesc.Buffer.NumElements          = numElements;
    uavDesc.Buffer.CounterOffsetInBytes = 0;
    uavDesc.Buffer.Flags                = D3D12_BUFFER_UAV_FLAG_NONE;
    uavDesc.Buffer.StructureByteStride  = byteStride;

    dxCommon_->GetDevice()->CreateUnorderedAccessView(resource, nullptr, &uavDesc, GetCPUDescriptorHandle(index));
}

