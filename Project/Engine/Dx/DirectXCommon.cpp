#include "DirectXCommon.h"
#include "2d/ImGuiManager.h"
#include "base/RtvManager.h"
#include "base/SrvManager.h"
#include "base/TextureManager.h"
#include "base/WinApp.h"
// dx
#include "DxCommand.h"
#include "DxCompiler.h"
#include "DxDevice.h"
#include "DxRenderTarget.h"
#include "DxSwapChain.h"

#include "Vector4.h"
#include <chrono>

#include <cassert>

#pragma comment(lib, "dxguid.lib")

DirectXCommon* DirectXCommon::GetInstance() {
    static DirectXCommon instance;
    return &instance;
}

void DirectXCommon::Init(WinApp* win, int32_t backBufferWidth, int32_t backBufferHeight) {

    winApp_           = win;
    backBufferWidth_  = backBufferWidth;
    backBufferHeight_ = backBufferHeight;

    srvManager_ = SrvManager::GetInstance();
    rtvManager_ = RtvManager::GetInstance();

    // DirectXクラス群の初期化
    InitDxClasses();
    SetupViewportAndScissor();
}

void DirectXCommon::InitDxClasses() {
    dxDevice_       = std::make_unique<DxDevice>();
    dxCommand_      = std::make_unique<DxCommand>();
    dxSwapChain_    = std::make_unique<DxSwapChain>();
    dxRenderTarget_ = std::make_unique<DxRenderTarget>();
    dxCompiler_     = std::make_unique<DxCompiler>();

    // 各Dxクラス初期化
    dxDevice_->Init();
    dxCommand_->Init(dxDevice_->GetDevice());
    dxSwapChain_->Init(dxDevice_->GetFactory(), dxCommand_->GetCommandQueue(), winApp_, backBufferWidth_, backBufferHeight_);

    dxCompiler_->Init();
}

void DirectXCommon::InitRenderingResources() {

    dxSwapChain_->CreateRenderTargetViews(rtvManager_);

    Vector4 clearColor = {0.2f, 0.2f, 0.2f, 1.0f};
    dxRenderTarget_->Init(dxDevice_->GetDevice(), rtvManager_, srvManager_, backBufferWidth_, backBufferHeight_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
}

void DirectXCommon::CreateGraphicPipeline() {}

void DirectXCommon::SetupViewportAndScissor() {
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

void DirectXCommon::PreDraw() {
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
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxRenderTarget_->GetDsvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

    dxCommand_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // 指定した色で画面全体をクリアする
    float clearColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; // 黒
    dxCommand_->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    ClearDepthBuffer();
}

void DirectXCommon::ClearDepthBuffer() {
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxRenderTarget_->GetDsvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    dxCommand_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::PostDraw() {
    // コマンド実行後のバリア
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource   = dxSwapChain_->GetSwapChainResource(dxSwapChain_->GetCurrentBackBufferIndex()).Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    dxCommand_->GetCommandList()->ResourceBarrier(1, &barrier);

    // 命令のクローズ
    dxCommand_->ExecuteCommand();

    // 画面に表示
    dxSwapChain_->Present();

    // コマンドの初期化
    dxCommand_->WaitForGPU();
    dxCommand_->ResetCommand();
}

void DirectXCommon::Finalize() {

    dxRenderTarget_->Finalize();
    dxSwapChain_->Finalize();
    dxCommand_->Finalize();
    dxDevice_->Finalize();
    dxCompiler_->Finalize();
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes) {
    //  リソース設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width            = sizeInBytes;
    resourceDesc.Height           = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels        = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Format           = DXGI_FORMAT_UNKNOWN;
    resourceDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags            = D3D12_RESOURCE_FLAG_NONE;

    //  ヒープ設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    Microsoft::WRL::ComPtr<ID3D12Resource> resource;

    //  バッファ生成
    hr_ = device->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr_));

    return resource;
}

void DirectXCommon::commandExecution(Microsoft::WRL::ComPtr<ID3D12Resource>& intermediateResource) {
    // GPUに転送するために、リソースバリアを設定
    D3D12_RESOURCE_BARRIER barrierDesc{};
    barrierDesc.Transition.pResource   = intermediateResource.Get();
    barrierDesc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrierDesc.Transition.StateAfter  = D3D12_RESOURCE_STATE_GENERIC_READ;
    dxCommand_->GetCommandList()->ResourceBarrier(1, &barrierDesc);
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::InitializeDescriptorHeap(
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

void DirectXCommon::PreRenderTexture() {}

void DirectXCommon::DepthBarrierTransition() {}

void DirectXCommon::PutTransitionBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES Before, D3D12_RESOURCE_STATES After) {
    D3D12_RESOURCE_BARRIER BarrierDesc{};
    BarrierDesc.Transition.pResource   = pResource;
    BarrierDesc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    BarrierDesc.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    BarrierDesc.Transition.StateBefore = Before;
    BarrierDesc.Transition.StateAfter  = After;
    dxCommand_->GetCommandList()->ResourceBarrier(1, &BarrierDesc);
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
    uint32_t descriptorSize,
    uint32_t index) {
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
    uint32_t descriptorSize,
    uint32_t index) {
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

Microsoft::WRL::ComPtr<ID3D12Device> DirectXCommon::GetDevice() const {
    return dxDevice_->GetDevice();
}

ID3D12GraphicsCommandList* DirectXCommon::GetCommandList() const {
    return dxCommand_->GetCommandList().Get();
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetRenderTextureGPUSrvHandle() const {
    return dxRenderTarget_->GetRenderTextureGPUSrvHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetRenderTextureCPUSrvHandle() const {
    return dxRenderTarget_->GetRenderTextureCPUSrvHandle();
}