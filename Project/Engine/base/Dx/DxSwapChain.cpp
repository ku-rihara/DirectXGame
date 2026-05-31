#include "DxSwapChain.h"

using namespace KetaEngine;
#include "Base/Descriptors/RtvManager.h"
#include "Base/WinApp.h"
#include <cassert>

void DxSwapChain::Init(
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory,
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
    WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

    commandQueue_ = commandQueue;

    // スワップチェーン設定
    desc_.Width            = backBufferWidth;
    desc_.Height           = backBufferHeight;
    desc_.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc_.SampleDesc.Count = 1;
    desc_.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc_.BufferCount      = 2;
    desc_.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc_.Flags            = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    // スワップチェーン作成
    hr_ = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        winApp->GetHwnd(),
        &desc_,
        nullptr,
        nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
    assert(SUCCEEDED(hr_));

    // 最大フレーム遅延を1に設定
    swapChain_->SetMaximumFrameLatency(1);
    waitableObject_ = swapChain_->GetFrameLatencyWaitableObject();

    // フェンスとイベントを一度だけ生成
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    commandQueue_->GetDevice(IID_PPV_ARGS(&device));
    hr_ = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr_));
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    assert(fenceEvent_ != nullptr);

    // Alt+Enterの無効化
    dxgiFactory->MakeWindowAssociation(winApp->GetHwnd(), DXGI_MWA_NO_ALT_ENTER);

    resourceStates_[0] = D3D12_RESOURCE_STATE_PRESENT;
    resourceStates_[1] = D3D12_RESOURCE_STATE_PRESENT;
}

void DxSwapChain::CreateRenderTargetViews(RtvManager* rtvManager) {

    // SwapChainからResourceを取得
    for (int i = 0; i < 2; i++) {
        resources_[i] = nullptr;
        hr_           = swapChain_->GetBuffer(i, IID_PPV_ARGS(&resources_[i]));
        assert(SUCCEEDED(hr_));
    }

    // RTV設定
    rtvDesc_.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    // RTVを作成   
    rtvManager->Create(rtvManager->Allocate(), resources_[0].Get(), &rtvDesc_);
    rtvManager->Create(rtvManager->Allocate(), resources_[1].Get(), &rtvDesc_);
}

void DxSwapChain::WaitForNextFrame() {
    if (!waitableObject_) {
        return;
    }
    // フレーム開始時にFLWOを待機する。

    WaitForSingleObjectEx(waitableObject_, INFINITE, FALSE);
}

void DxSwapChain::Present() {
    /// PRESENT状態に遷移
    swapChain_->Present(1, 0);

    // Present後、現在のバックバッファはPRESENT状態になる
    UpdateResourceState(GetCurrentBackBufferIndex(), D3D12_RESOURCE_STATE_PRESENT);
}

// リソース状態管理
void DxSwapChain::UpdateResourceState(const UINT& index, const D3D12_RESOURCE_STATES& state) {
    if (index < 2) {
        resourceStates_[index] = state;
    }
}

D3D12_RESOURCE_STATES DxSwapChain::GetResourceState(const UINT& index) const {
    if (index < 2) {
        return resourceStates_[index];
    } else {
        return D3D12_RESOURCE_STATE_PRESENT;
    }
}

void DxSwapChain::WaitForGPU() {
    if (!commandQueue_ || !fence_ || !fenceEvent_) {
        return;
    }

    ++fenceValue_;
    commandQueue_->Signal(fence_.Get(), fenceValue_);

    if (fence_->GetCompletedValue() < fenceValue_) {
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
}

void DxSwapChain::Finalize() {
  
    WaitForGPU();

    for (int i = 0; i < 2; i++) {
        if (resources_[i]) {
            resources_[i].Reset();
        }
    }
    if (swapChain_) {
        swapChain_.Reset();
    }
    
    if (waitableObject_) {
        CloseHandle(waitableObject_);
        waitableObject_ = nullptr;
    }

    if (fenceEvent_) {
        CloseHandle(fenceEvent_);
        fenceEvent_ = nullptr;
    }
    fence_.Reset();

    commandQueue_.Reset();
}