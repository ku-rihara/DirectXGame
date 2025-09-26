#include "UavManager.h"
#include <cassert>
#include <d3d12.h>

// 最大RTV数の定義
const uint32_t UavManager::kMaxCount = 4;

// シングルトンインスタンスの取得
UavManager* UavManager::GetInstance() {
    static UavManager instance;
    return &instance;
}

///=========================================
/// 初期化
///=========================================
void UavManager::Init(DirectXCommon* directXCommon) {
    dxCommon_       = directXCommon;
    descriptorHeap_ = dxCommon_->InitializeDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxCount, false);
    descriptorSize_ = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    useIndex_       = 0;
}

///=========================================
/// アロケータ
///=========================================
uint32_t UavManager::Allocate() {

    assert(IsAbleSecure());

    static bool isFirst = true;

    if (isFirst) {
        isFirst = false;
        return 0;
    }

    useIndex_++; // 2回目以降に加算
    return useIndex_;
}

bool UavManager::IsAbleSecure() {
    if (useIndex_ >= UavManager::kMaxCount) {
        return false;
    } else {
        return true;
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE UavManager::GetCPUDescriptorHandle(const uint32_t& index) {
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize_ * index);
    return handleCPU;
}

void UavManager::Create(const uint32_t& index, ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc) {

    dxCommon_->GetDevice()->CreateRenderTargetView(resource, desc, GetCPUDescriptorHandle(index));
}