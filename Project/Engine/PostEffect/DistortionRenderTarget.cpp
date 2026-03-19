#include "DistortionRenderTarget.h"

using namespace KetaEngine;
#include "Base/Descriptors/RtvManager.h"
#include "Base/Descriptors/SrvManager.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/Dx/DxResourceBarrier.h"
#include <cassert>

///============================================================
/// 初期化
///============================================================
void DistortionRenderTarget::Init(DirectXCommon* dxCommon, uint32_t width, uint32_t height) {
    dxCommon_ = dxCommon;
    CreateResource(width, height);
    CreateRTV();
    CreateSRV();

    // バリア管理に登録（初期状態: RENDER_TARGET）
    dxCommon_->GetResourceBarrier()->RegisterResource(resource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
}

///============================================================
/// リソース作成（R16G16_FLOAT）
///============================================================
void DistortionRenderTarget::CreateResource(uint32_t width, uint32_t height) {
    D3D12_RESOURCE_DESC desc{};
    desc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width              = width;
    desc.Height             = height;
    desc.DepthOrArraySize   = 1;
    desc.MipLevels          = 1;
    desc.Format             = DXGI_FORMAT_R16G16_FLOAT;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format   = DXGI_FORMAT_R16G16_FLOAT;
    clearValue.Color[0] = kClearColor[0];
    clearValue.Color[1] = kClearColor[1];
    clearValue.Color[2] = kClearColor[2];
    clearValue.Color[3] = kClearColor[3];

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue,
        IID_PPV_ARGS(&resource_));

    assert(SUCCEEDED(hr) && "DistortionRenderTarget: リソース作成失敗");
    resource_->SetName(L"DistortionRT");
}

///============================================================
/// RTV作成
///============================================================
void DistortionRenderTarget::CreateRTV() {
    auto* rtvManager = RtvManager::GetInstance();
    rtvIndex_        = rtvManager->Allocate();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format        = DXGI_FORMAT_R16G16_FLOAT;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    rtvManager->Create(rtvIndex_, resource_.Get(), &rtvDesc);
    rtvHandle_ = rtvManager->GetCPUDescriptorHandle(rtvIndex_);
}

///============================================================
/// SRV作成
///============================================================
void DistortionRenderTarget::CreateSRV() {
    auto* srvManager = SrvManager::GetInstance();
    uint32_t index   = srvManager->Allocate();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format                  = DXGI_FORMAT_R16G16_FLOAT;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels     = 1;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    srvManager->CreateForTexture2D(index, resource_.Get(), srvDesc);
    gpuSrvHandle_ = srvManager->GetGPUDescriptorHandle(index);
}

///============================================================
/// 歪みパス開始（RTをセット＆クリア）
///============================================================
void DistortionRenderTarget::BeginDistortionPass(ID3D12GraphicsCommandList* commandList,
                                                  D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
    // RENDER_TARGET状態へ遷移
    dxCommon_->GetResourceBarrier()->Transition(
        commandList, resource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

    // 歪みRTをセット（シーンRTの代わりにこちらへ書き込む）
    commandList->OMSetRenderTargets(1, &rtvHandle_, FALSE, &dsvHandle);

    // UVオフセット(0,0)でクリア = 歪みなしの初期状態
    commandList->ClearRenderTargetView(rtvHandle_, kClearColor, 0, nullptr);
}

///============================================================
/// 歪みパス終了（SRVとして読めるよう遷移）
///============================================================
void DistortionRenderTarget::EndDistortionPass(ID3D12GraphicsCommandList* commandList) {
    dxCommon_->GetResourceBarrier()->Transition(
        commandList, resource_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

///============================================================
/// 終了処理
///============================================================
void DistortionRenderTarget::Finalize() {
    if (resource_) {
        dxCommon_->GetResourceBarrier()->UnregisterResource(resource_.Get());
    }
    resource_.Reset();
}
