#include "DxRenderTarget.h"
#include "base/DsvManager.h"
#include "base/RtvManager.h"
#include "base/SrvManager.h"
#include "DxCommand.h"
#include "DxSwapChain.h"
#include"DxDepthBuffer.h"
#include <cassert>

///==========================================================
/// 初期化
///==========================================================
void DxRenderTarget::Init(
    Microsoft::WRL::ComPtr<ID3D12Device> device, DxDepthBuffer*depthBuffer, RtvManager* rtvManager, SrvManager* srvManager,
    DxCommand* dxCommand, DxSwapChain* dxSwapChain, uint32_t width, uint32_t height) {

    rtvManager_  = rtvManager;
    srvManager_  = srvManager;
    dxCommand_   = dxCommand;
    dxSwapChain_ = dxSwapChain;

    backBufferHeight_ = height;
    backBufferWidth_  = width;

    depthBuffer_ = depthBuffer;

    clearColor_ = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

    // レンダーテクスチャリソースの作成
    renderTextureResource_ = CreateRenderTextureResource(device, backBufferWidth_, backBufferHeight_,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);

    // 深度バッファの初期化
    depthBuffer_->Init(device, DsvManager::GetInstance(), srvManager_, backBufferWidth_, backBufferHeight_);

    // レンダーテクスチャのRTV作成
    CreateRenderTextureRTV();

    // レンダーテクスチャのSRV作成
    CreateRenderTextureSRV();

    // ビューポートとシザー矩形の初期化
    SetupViewportAndScissor();

    // 初期状態をRENDER_TARGETに設定
    renderTextureCurrentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
}

///==========================================================
/// レンダーテクスチャRTV作成
///==========================================================
void DxRenderTarget::CreateRenderTextureRTV() {
    renderTextureRtvIndex_ = rtvManager_->Allocate();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    rtvManager_->CreateRTV(renderTextureRtvIndex_, renderTextureResource_.Get(), &rtvDesc);
}

///==========================================================
/// レンダーテクスチャSRV作成
///==========================================================
void DxRenderTarget::CreateRenderTextureSRV() {
    uint32_t srvIndex = srvManager_->Allocate();

    renderTextureGPUSrvHandle_ = srvManager_->GetGPUDescriptorHandle(srvIndex);
    renderTextureCPUSrvHandle_ = srvManager_->GetCPUDescriptorHandle(srvIndex);

    // SRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Texture2D.MipLevels     = 1;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    // SRV生成
    srvManager_->CreateSRVforTexture2D(srvIndex, renderTextureResource_.Get(), srvDesc);
}

///==========================================================
/// レンダーテクスチャリソース作成
///==========================================================
Microsoft::WRL::ComPtr<ID3D12Resource> DxRenderTarget::CreateRenderTextureResource(
    Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height,
    DXGI_FORMAT format, const Vector4& clearColor) {

    // リソース設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Width              = width;
    resourceDesc.Height             = height;
    resourceDesc.DepthOrArraySize   = 1;
    resourceDesc.MipLevels          = 1;
    resourceDesc.Format             = format;
    resourceDesc.SampleDesc.Count   = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    clearValue_.Format   = format;
    clearValue_.Color[0] = clearColor.x;
    clearValue_.Color[1] = clearColor.y;
    clearValue_.Color[2] = clearColor.z;
    clearValue_.Color[3] = clearColor.w;

    // ヒープ設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // リソース生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr                                      = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue_,
        IID_PPV_ARGS(&resource));

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create render texture resource heap.\n");
        return nullptr;
    }
    return resource;
}

///==========================================================
/// レンダーテクスチャへの描画準備
///==========================================================
void DxRenderTarget::PreRenderTexture() {
    // レンダーテクスチャをRENDER_TARGET状態に遷移
    if (renderTextureCurrentState_ != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        PutTransitionBarrier(renderTextureResource_.Get(), renderTextureCurrentState_,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    // 深度バッファをDEPTH_WRITE状態に遷移
    depthBuffer_->TransitionState(dxCommand_->GetCommandList(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

    // レンダーターゲットと深度ステンシルビューのハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(renderTextureRtvIndex_);
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthBuffer_->GetDsvHandle();

    // レンダーターゲット設定
    dxCommand_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // レンダーターゲットと深度ステンシルビューをクリア
    dxCommand_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearValue_.Color, 0, nullptr);
    depthBuffer_->Clear(dxCommand_->GetCommandList());

    // ビューポートとシザー矩形を設定
    dxCommand_->GetCommandList()->RSSetViewports(1, &viewport_);
    dxCommand_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
}

///==========================================================
/// メイン描画準備
///==========================================================
void DxRenderTarget::PreDraw() {
    // バックバッファのインデックス取得
    backBufferIndex_ = dxSwapChain_->GetCurrentBackBufferIndex();

    // 深度バッファをシェーダーリソース状態に遷移
    depthBuffer_->TransitionState(dxCommand_->GetCommandList(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // レンダーテクスチャをシェーダーリソース状態に遷移
    if (renderTextureCurrentState_ != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
        PutTransitionBarrier(renderTextureResource_.Get(), renderTextureCurrentState_,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }

    // SwapChainリソースの現在の状態を取得
    D3D12_RESOURCE_STATES currentSwapChainState = dxSwapChain_->GetResourceState(backBufferIndex_);

    // SwapChainリソースをRENDER_TARGET状態に遷移
    if (currentSwapChainState != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        PutTransitionBarrier(dxSwapChain_->GetSwapChainResource(backBufferIndex_).Get(),
            currentSwapChainState, D3D12_RESOURCE_STATE_RENDER_TARGET);
        dxSwapChain_->UpdateResourceState(backBufferIndex_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    // レンダーターゲットと深度ステンシルビューのハンドル取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(backBufferIndex_);
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthBuffer_->GetDsvHandle();

    // レンダーターゲット設定
    dxCommand_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // レンダーターゲットをクリア
    dxCommand_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearValue_.Color, 0, nullptr);

    // ビューポートとシザー矩形を設定
    dxCommand_->GetCommandList()->RSSetViewports(1, &viewport_);
    dxCommand_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
}

///==========================================================
/// 描画後の状態遷移
///==========================================================
void DxRenderTarget::PostDrawTransitionBarrier() {
    UINT currentIndex = dxSwapChain_->GetCurrentBackBufferIndex();

    // 深度バッファをDEPTH_WRITE状態に遷移
    depthBuffer_->TransitionState(dxCommand_->GetCommandList(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

    // SwapChainリソースをPRESENT状態に遷移
    PutTransitionBarrier(dxSwapChain_->GetSwapChainResource(currentIndex).Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    dxSwapChain_->UpdateResourceState(currentIndex, D3D12_RESOURCE_STATE_PRESENT);
}

///==========================================================
/// ビューポートとシザー矩形設定
///==========================================================
void DxRenderTarget::SetupViewportAndScissor() {
    // ビューポート設定
    viewport_.Width    = float(backBufferWidth_);
    viewport_.Height   = float(backBufferHeight_);
    viewport_.TopLeftX = 0;
    viewport_.TopLeftY = 0;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;

    // シザー矩形
    scissorRect_.left   = 0;
    scissorRect_.right  = int32_t(backBufferWidth_);
    scissorRect_.top    = 0;
    scissorRect_.bottom = int32_t(backBufferHeight_);
}

///==========================================================
/// リソース状態遷移バリア
///==========================================================
void DxRenderTarget::PutTransitionBarrier(ID3D12Resource* pResource,
    D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After) {

    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource   = pResource;
    barrier.Transition.StateBefore = Before;
    barrier.Transition.StateAfter  = After;

    dxCommand_->GetCommandList()->ResourceBarrier(1, &barrier);

    // レンダーテクスチャの状態更新
    if (pResource == renderTextureResource_.Get()) {
        renderTextureCurrentState_ = After;
    }
}

///==========================================================
/// 終了処理
///==========================================================
void DxRenderTarget::Finalize() {
    renderTextureResource_.Reset();
}