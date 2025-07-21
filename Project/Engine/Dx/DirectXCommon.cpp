#include "DirectXCommon.h"
#include "base/RtvManager.h"
#include "base/SrvManager.h"
#include "base/WinApp.h"
#include "Frame/Frame.h"
// dx
#include "DxCommand.h"
#include "DxCompiler.h"
#include "DxDevice.h"
#include "DxRenderTarget.h"
#include "DxSwapChain.h"
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

    Frame::Init();

    // DirectXクラス群の初期化
    InitDxClasses();
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
    dxRenderTarget_->Init(dxDevice_->GetDevice(), rtvManager_, srvManager_, dxCommand_.get(), dxSwapChain_.get(), backBufferWidth_, backBufferHeight_);
}

void DirectXCommon::PreDraw() {
    dxRenderTarget_->PreDraw();
}

void DirectXCommon::PreRenderTexture() {
    dxRenderTarget_->PreRenderTexture();
}

void DirectXCommon::PostDraw() {
    // TransitionBarrierを張る
    dxRenderTarget_->PostDrawTransitionBarrier();

    // 命令のクローズ
    dxCommand_->ExecuteCommand();

    // 画面に表示
    dxSwapChain_->Present();

    // コマンドの初期化
    dxCommand_->WaitForGPU();
    Frame::Update();
    dxCommand_->ResetCommand();
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

void DirectXCommon::DepthBarrierTransition() {}

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

void DirectXCommon::Finalize() {

    dxCompiler_->Finalize();
    dxCommand_->Finalize();
    dxDevice_->Finalize();
    dxSwapChain_->Finalize();
    dxRenderTarget_->Finalize();

    CloseWindow(winApp_->GetHwnd());
}
