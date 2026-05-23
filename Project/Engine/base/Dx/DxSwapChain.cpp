#include "DxSwapChain.h"

using namespace KetaEngine;
#include "Base/Descriptors/RtvManager.h"
#include "Base/WinApp.h"
#include <cassert>

// Tearingサポートフラグ
static bool g_TearingSupported = false;

void DxSwapChain::Init(
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory,
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
    WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

    commandQueue_ = commandQueue;

    // Tearingサポートの確認
    Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
    if (SUCCEEDED(dxgiFactory.As(&factory5))) {
        BOOL allowTearing = FALSE;
        if (SUCCEEDED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))) && allowTearing) {
            g_TearingSupported = true;
        }
    }

    // スワップチェーン設定
    desc_.Width            = backBufferWidth;
    desc_.Height           = backBufferHeight;
    desc_.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc_.SampleDesc.Count = 1;
    desc_.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc_.BufferCount      = 2;
    desc_.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    // Tearing許可と同時にFRAME_LATENCY_WAITABLE_OBJECTフラグをセットして遅延を制御できるようにする
    desc_.Flags            = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | (g_TearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

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

void DxSwapChain::Present() {
    if (waitableObject_) {
        // 次のフレームを描画して良いタイミングまで待機
        WaitForSingleObject(waitableObject_, INFINITE);
    }

    UINT presentFlags = g_TearingSupported ? DXGI_PRESENT_ALLOW_TEARING : 0;
    // VSyncをオフ (0) にして Tearing を許可
    swapChain_->Present(0, presentFlags);

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
    
    if (waitableObject_) {
        CloseHandle(waitableObject_);
        waitableObject_ = nullptr;
    }

    commandQueue_.Reset();
}
