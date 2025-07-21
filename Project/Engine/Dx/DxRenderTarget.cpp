#include "DxRenderTarget.h"
#include "base/RtvManager.h"
#include "base/SrvManager.h"
#include "DxCommand.h"
#include "DxSwapChain.h"
#include <cassert>

///==========================================================
/// 初期化
///==========================================================
void DxRenderTarget::Init(
    Microsoft::WRL::ComPtr<ID3D12Device> device, RtvManager* rtvManager, SrvManager* srvManager, DxCommand* dxCommand, DxSwapChain* dxSwapChain, uint32_t width, uint32_t height) {

    rtvManager_  = rtvManager;
    srvManager_  = srvManager;
    dxCommand_   = dxCommand;
    dxSwapChain_ = dxSwapChain;

    backBufferHeight_ = height;
    backBufferWidth_  = width;

    // DSVのデスクリプタサイズを取得
    descriptorSizeDSV_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    // レンダーテクスチャリソースの作成
    renderTextureResource_ = CreateRenderTextureResource(device, backBufferWidth_, backBufferHeight_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);

    // 深度バッファの作成
    CreateDepthBuffer(device);

    // SRVハンドルの作成
    CreateSrvHandle();

    // 初期状態をRENDER_TARGETに設定
    renderTextureCurrentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
}

///==========================================================
/// 深度バッファ作成
///==========================================================
void DxRenderTarget::CreateDepthBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device) {
    // 深度ステンシルリソース作成
    depthStencilResource_ = CreateDepthStencilTextureResource(device, backBufferWidth_, backBufferHeight_);

    // DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    dsvDescriptorHeap_ = InitializeDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture

    // DSVHeapの先頭にDSVを作る
    device->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

///==========================================================
/// SRVハンドル作成
///==========================================================
void DxRenderTarget::CreateSrvHandle() {
    uint32_t srvIndex = srvManager_->Allocate();

    renderTextureGPUSrvHandle_ = srvManager_->GetGPUDescriptorHandle(srvIndex);
    renderTextureCPUSrvHandle_ = srvManager_->GetCPUDescriptorHandle(srvIndex);

    // SRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Texture2D.MipLevels     = 1;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    /// Srvの生成
    srvManager_->CreateSRVforTexture2D(
        srvIndex,
        renderTextureResource_.Get(),
        srvDesc);
}

///==========================================================
/// 終了処理
///==========================================================
void DxRenderTarget::Finalize() {
    renderTextureResource_.Reset();
    depthStencilResource_.Reset();
    dsvDescriptorHeap_.Reset();
}

///==========================================================
/// レンダーテクスチャリソース作成
///==========================================================
Microsoft::WRL::ComPtr<ID3D12Resource> DxRenderTarget::CreateRenderTextureResource(
    Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {

    // 1. metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    resourceDesc.Width              = width;
    resourceDesc.Height             = height;
    resourceDesc.DepthOrArraySize   = 1; // 必須
    resourceDesc.MipLevels          = 1; // 最低1は必要
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

    // 2. 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 3. Resourceを生成してreturnする
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr                                      = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue_,
        IID_PPV_ARGS(&resource));

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to createRender Texture Resource heap.\n");
        return nullptr;
    }
    return resource;
}

///==========================================================
/// 深度ステンシルテクスチャリソース作成
///==========================================================
Microsoft::WRL::ComPtr<ID3D12Resource> DxRenderTarget::CreateDepthStencilTextureResource(
    Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height) {

    // 生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width            = width;
    resourceDesc.Height           = height;
    resourceDesc.MipLevels        = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // 利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
    depthClearValue.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Resoureceの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr                                      = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&resource));

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create depth senthil resource heap.\n");
        return nullptr;
    }
    return resource.Get();
}

///==========================================================
/// ディスクリプタヒープ初期化
///==========================================================
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DxRenderTarget::InitializeDescriptorHeap(
    Microsoft::WRL::ComPtr<ID3D12Device> device,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType,
    UINT numDescriptors,
    bool shaderVisible) {

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
    DescriptorHeapDesc.Type           = heapType; // レンダーターゲットビュー用
    DescriptorHeapDesc.NumDescriptors = numDescriptors; // 指定された数
    DescriptorHeapDesc.Flags          = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr                        = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

    // ディスクリプタヒープが作れなかったので起動出来ない
    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create descriptor heap.\n");
        return nullptr;
    }
    return descriptorHeap;
}

void DxRenderTarget::PreDraw() {

    // コマンド実行前のバリア
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource   = dxSwapChain_->GetSwapChainResource(dxSwapChain_->GetCurrentBackBufferIndex()).Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
    dxCommand_->GetCommandList()->ResourceBarrier(1, &barrier);

    // ビューポートとシザー矩形の設定
    dxCommand_->GetCommandList()->RSSetViewports(1, &viewport_);
    dxCommand_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);

    // レンダーターゲットを設定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(dxSwapChain_->GetCurrentBackBufferIndex());
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

    dxCommand_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // 指定した色で画面全体をクリアする
    float clearColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; // 黒
    dxCommand_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    ClearDepthBuffer();
}

void DxRenderTarget::ClearDepthBuffer() {
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    dxCommand_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DxRenderTarget::PreRenderTexture() {
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

    // 現在の状態がすでにRENDER_TARGETでない場合のみ遷移
    if (renderTextureCurrentState_ != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        PutTransitionBarrier(renderTextureResource_.Get(),
            renderTextureCurrentState_, D3D12_RESOURCE_STATE_RENDER_TARGET);
        renderTextureCurrentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvManager_->GetCPUDescriptorHandle(2);

    dxCommand_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    // レンダーターゲットと深度ステンシルビューをクリア
    dxCommand_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearValue_.Color, 0, nullptr);
    dxCommand_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    // ビューポートとシザー矩形を設定
    dxCommand_->GetCommandList()->RSSetViewports(1, &viewport_);
    dxCommand_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);
}

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