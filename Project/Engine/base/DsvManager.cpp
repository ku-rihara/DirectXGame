#include "DsvManager.h"
#include <cassert>
#include <d3d12.h>

// 最大RTV数の定義
const uint32_t DsvManager::kMaxCount = 2;

// シングルトンインスタンスの取得
DsvManager* DsvManager::GetInstance() {
    static DsvManager instance;
    return &instance;
}

///=========================================
/// 初期化
///=========================================
void DsvManager::Init(DirectXCommon* directXCommon) {
    dxCommon_       = directXCommon;
    descriptorHeap_ = dxCommon_->InitializeDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxCount, false);
    descriptorSize_ = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    useIndex_       = 0;
}

///=========================================
/// アロケータ
///=========================================
uint32_t DsvManager::Allocate() {

    assert(IsAbleSecure());

    static bool isFirst = true;

    if (isFirst) {
        isFirst = false;
        return 0;
    }

    useIndex_++; // 2回目以降に加算
    return useIndex_;
}

bool DsvManager::IsAbleSecure() {
    if (useIndex_ >= DsvManager::kMaxCount) {
        return false;
    } else {
        return true;
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE DsvManager::GetCPUDescriptorHandle(uint32_t index) {
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize_ * index);
    return handleCPU;
}

void DsvManager::CreateDSV(uint32_t index, ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC* desc) {

    dxCommon_->GetDevice()->CreateDepthStencilView(resource, desc, GetCPUDescriptorHandle(index));
}