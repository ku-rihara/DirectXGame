#include "DirectXCommon.h"

using namespace KetaEngine;
#include "Base/Descriptors/RtvManager.h"
#include "Base/Descriptors/SrvManager.h"
#include "Base/WinApp.h"
#include "Frame/Frame.h"
// dx
#include "DxCommand.h"
#include "DxCompiler.h"
#include "DxDepthBuffer.h"
#include "DxDevice.h"
#include "DxRenderTarget.h"
#include "DxSwapChain.h"
#include"DxResourceBarrier.h"
#include <cassert>

#pragma comment(lib, "dxguid.lib")

DirectXCommon* DirectXCommon::GetInstance() {
    static DirectXCommon instance;
    return &instance;
}

void DirectXCommon::Init(WinApp* win, int32_t backBufferWidth, int32_t backBufferHeight) {
    // ウィンドウサイズと描画サイズが同じ場合
    Init(win, backBufferWidth, backBufferHeight, backBufferWidth, backBufferHeight);
}

void DirectXCommon::Init(WinApp* win, int32_t windowWidth, int32_t windowHeight, int32_t renderWidth, int32_t renderHeight) {

    winApp_           = win;
    windowWidth_      = windowWidth;
    windowHeight_     = windowHeight;
    backBufferWidth_  = renderWidth;
    backBufferHeight_ = renderHeight;

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
    depthBuffer_    = std::make_unique<DxDepthBuffer>();
    resourceBarrier_ = std::make_unique<DxResourceBarrier>();

    // 各Dxクラス初期化
    dxDevice_->Init();
    dxCommand_->Init(dxDevice_->GetDevice());
    // SwapChainはウィンドウサイズで作成
    dxSwapChain_->Init(dxDevice_->GetFactory(), dxCommand_->GetCommandQueue(), winApp_, windowWidth_, windowHeight_);

    dxCompiler_->Init();
}

void DirectXCommon::InitRenderingResources() {

    dxSwapChain_->CreateRenderTargetViews(rtvManager_);

    // SwapChainのバックバッファを登録
     for (UINT i = 0; i < 2; ++i) { 
        resourceBarrier_->RegisterResource(
            dxSwapChain_->GetSwapChainResource(i).Get(),
            D3D12_RESOURCE_STATE_PRESENT);
    }

    // renderTarget初期化
    dxRenderTarget_->SetUseClasses(
        depthBuffer_.get(), rtvManager_, srvManager_, dxCommand_.get(), dxSwapChain_.get(),resourceBarrier_.get());
    dxRenderTarget_->Init(dxDevice_->GetDevice(),backBufferWidth_, backBufferHeight_);
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

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateBufferResource(
    Microsoft::WRL::ComPtr<ID3D12Device> device,
    const std::size_t& sizeInBytes,
    const ViewType& viewType) {
    // リソース記述
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width            = static_cast<UINT64>(sizeInBytes);
    resourceDesc.Height           = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels        = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Format           = DXGI_FORMAT_UNKNOWN;
    resourceDesc.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // ヒーププロパティ
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    //初期ステート
    D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;

    switch (viewType) {
    case ViewType::UnorderedAccess:
        // UAV用
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        heapProps.Type     = D3D12_HEAP_TYPE_DEFAULT;
        initialState       = D3D12_RESOURCE_STATE_COMMON;
        break;

    case ViewType::ShaderResource:
    default:
        // SRV用
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        heapProps.Type     = D3D12_HEAP_TYPE_UPLOAD;
        initialState       = D3D12_RESOURCE_STATE_GENERIC_READ;
        break;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        initialState, nullptr, IID_PPV_ARGS(&resource));

    assert(SUCCEEDED(hr) && "CreateCommittedResource failed");
    if (FAILED(hr)) {
        return nullptr;
    }

    return resource;
}
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::InitializeDescriptorHeap(
    Microsoft::WRL::ComPtr<ID3D12Device> device, const D3D12_DESCRIPTOR_HEAP_TYPE& heapType,
    const UINT& numDescriptors, bool shaderVisible) {

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
    return dxCommand_->GetCommandList();
}

void DirectXCommon::Finalize() {

    dxCompiler_->Finalize();
    dxCommand_->Finalize();
    dxDevice_->Finalize();
    dxSwapChain_->Finalize();
    dxRenderTarget_->Finalize();

    CloseWindow(winApp_->GetHwnd());
}
