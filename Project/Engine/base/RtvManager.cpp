#include "RtvManager.h"
#include <cassert>

// 最大SRV数の定義
const uint32_t RtvManager::kMaxCount = 3;

// シングルトンインスタンスの取得
RtvManager* RtvManager::GetInstance() {
    static RtvManager instance; // 静的インスタンス
    return &instance;
}

///=========================================
/// 初期化
///=========================================
void RtvManager::Init(DirectXCommon* directXCommon) {
    dxCommon_       = directXCommon;
    descriptorHeap_ = dxCommon_->InitializeDescriptorHeap(dxCommon_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxCount, false);
    descriptorSize_ = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    useIndex_       = 0;
}

///=========================================
/// アロケータ
///=========================================
uint32_t RtvManager::Allocate() {

    assert(IsAbleSecure());

    static bool isFirst = true;

    if (isFirst) {
        isFirst = false;
        return useIndex_;
    }

    useIndex_++; // 2回目以降に加算
    return useIndex_;
}

bool RtvManager::IsAbleSecure() {
    if (useIndex_ >= RtvManager::kMaxCount) {
        return false;
    } else {
        return true;
    }
}

///==========================================
/// 描画前処理
///==========================================
void RtvManager::PreDraw() {
    // コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = {descriptorHeap_.Get()};
    dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());
}

D3D12_CPU_DESCRIPTOR_HANDLE RtvManager::GetCPUDescriptorHandle(uint32_t index) {
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize_ * index);
    return handleCPU;
}

void RtvManager::CreateRTV(uint32_t index, ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* desc) {

    dxCommon_->GetDevice()->CreateRenderTargetView(resource, desc, GetCPUDescriptorHandle(index));
}