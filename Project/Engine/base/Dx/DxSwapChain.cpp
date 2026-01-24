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

    // スワップチェーン作成
    hr_ = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        winApp->GetHwnd(),
        &desc_,
        nullptr,
        nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
    assert(SUCCEEDED(hr_));

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

void DxSwapChain::Present() {
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
    if (!commandQueue_) {
        return;
    }

 
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    UINT64 fenceValue = 1;

   
    Microsoft::WRL::ComPtr<ID3D12Device> device;
    hr_ = commandQueue_->GetDevice(IID_PPV_ARGS(&device));
    if (FAILED(hr_)) {
        return;
    }

    hr_ = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    if (FAILED(hr_)) {
        return;
    }

   
    hr_ = commandQueue_->Signal(fence.Get(), fenceValue);
    if (FAILED(hr_)) {
        return;
    }

    if (fence->GetCompletedValue() < fenceValue) {
        HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEvent != nullptr) {
            hr_ = fence->SetEventOnCompletion(fenceValue, fenceEvent);
            if (SUCCEEDED(hr_)) {
                WaitForSingleObject(fenceEvent, INFINITE);
            }
            CloseHandle(fenceEvent);
        }
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

    commandQueue_.Reset();
}
