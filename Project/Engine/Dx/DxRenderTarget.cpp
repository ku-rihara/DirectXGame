#include "DxRenderTarget.h"
#include "base/DsvManager.h"
#include "base/RtvManager.h"
#include "base/SrvManager.h"
#include "DxCommand.h"
#include "DxDepthBuffer.h"
#include "DxResourceBarrier.h"
#include "DxSwapChain.h"
#include <cassert>

///==========================================================
/// 初期化
///==========================================================
void DxRenderTarget::Init(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height) {

    backBufferHeight_ = height;
    backBufferWidth_  = width;

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

    dxResourceBarrier_->RegisterResource(renderTextureResource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
}

///==========================================================
/// レンダーテクスチャRTV作成
///==========================================================
void DxRenderTarget::CreateRenderTextureRTV() {
    renderTextureRtvIndex_ = rtvManager_->Allocate();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    rtvManager_->Create(renderTextureRtvIndex_, renderTextureResource_.Get(), &rtvDesc);
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
    srvManager_->CreateForTexture2D(srvIndex, renderTextureResource_.Get(), srvDesc);
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

void DxRenderTarget::PreRenderTexture() {

    // ----------------------------------------------
    // レンダーテクスチャをRENDER_TARGET状態に遷移
    // ----------------------------------------------
    dxResourceBarrier_->Transition(dxCommand_->GetCommandList(), renderTextureResource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

    // ----------------------------------------------
    // 深度バッファをDEPTH_WRITE状態に遷移
    // ----------------------------------------------
    depthBuffer_->TransitionState(dxCommand_->GetCommandList(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

    // ----------------------------------------------
    // レンダーターゲット設定
    // ----------------------------------------------
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(renderTextureRtvIndex_);
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthBuffer_->GetDsvHandle();

    dxCommand_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // ----------------------------------------------
    // クリア処理
    // ----------------------------------------------
    dxCommand_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearValue_.Color, 0, nullptr);
    depthBuffer_->Clear(dxCommand_->GetCommandList());

    // ----------------------------------------------
    // ビューポート・シザー設定
    // ----------------------------------------------
    dxCommand_->GetCommandList()->RSSetViewports(1, &viewport_);
    dxCommand_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
}

void DxRenderTarget::PreDraw() {

    // ----------------------------------------------
    // オフスクリーンリソースをシェーダーリソース状態に遷移
    // ----------------------------------------------

    // レンダーテクスチャをシェーダーリソースに変更
    dxResourceBarrier_->Transition(dxCommand_->GetCommandList(),
        renderTextureResource_.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // 深度バッファをシェーダーリソースに変更
    depthBuffer_->TransitionState(dxCommand_->GetCommandList(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // ----------------------------------------------
    // バックバッファをレンダーターゲット状態に遷移
    // ----------------------------------------------
    backBufferIndex_ = dxSwapChain_->GetCurrentBackBufferIndex();

    dxResourceBarrier_->Transition(dxCommand_->GetCommandList(),
        dxSwapChain_->GetSwapChainResource(backBufferIndex_).Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    // ----------------------------------------------
    // バックバッファをレンダーターゲットに設定
    // ----------------------------------------------
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(backBufferIndex_);

    dxCommand_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // ----------------------------------------------
    //  バックバッファをクリア
    // ----------------------------------------------
    dxCommand_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearValue_.Color, 0, nullptr);

    // ----------------------------------------------
    // ビューポート・シザー設定
    // ----------------------------------------------
    dxCommand_->GetCommandList()->RSSetViewports(1, &viewport_);
    dxCommand_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
}

///==========================================================
/// 描画終了後のバリア設定
///==========================================================
void DxRenderTarget::PostDrawTransitionBarrier() {
    UINT currentIndex = dxSwapChain_->GetCurrentBackBufferIndex();

    // ----------------------------------------------
    //  バックバッファをPRESENT状態に遷移
    // ----------------------------------------------
    dxResourceBarrier_->Transition(dxCommand_->GetCommandList(),
        dxSwapChain_->GetSwapChainResource(currentIndex).Get(),
        D3D12_RESOURCE_STATE_PRESENT);

    // ----------------------------------------------
    //  オフスクリーンリソースを次フレーム用に準備
    // ----------------------------------------------

    dxResourceBarrier_->Transition(dxCommand_->GetCommandList(),
        renderTextureResource_.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    // 深度バッファを次フレーム用に準備
    depthBuffer_->TransitionState(dxCommand_->GetCommandList(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
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
/// 終了処理
///==========================================================
void DxRenderTarget::Finalize() {
    if (renderTextureResource_) {
        dxResourceBarrier_->UnregisterResource(renderTextureResource_.Get());
    }
    renderTextureResource_.Reset();
}

void DxRenderTarget::SetUseClasses(DxDepthBuffer* depthBuffer, RtvManager* rtvManager, SrvManager* srvManager,
    DxCommand* dxCommand, DxSwapChain* dxSwapChain, DxResourceBarrier* resourceBarrier) {
    rtvManager_        = rtvManager;
    srvManager_        = srvManager;
    dxCommand_         = dxCommand;
    dxSwapChain_       = dxSwapChain;
    depthBuffer_       = depthBuffer;
    dxResourceBarrier_ = resourceBarrier;
}
