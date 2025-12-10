#include "DxDepthBuffer.h"

using namespace KetaEngine;
#include "base/DsvManager.h"
#include "base/SrvManager.h"
#include <cassert>

void DxDepthBuffer::Init(Microsoft::WRL::ComPtr<ID3D12Device> device, DsvManager* dsvManager,
    SrvManager* srvManager,uint32_t width,uint32_t height) {
    dsvManager_ = dsvManager;
    srvManager_ = srvManager;
    width_      = width;
    height_     = height;

    // 深度ステンシルリソース作成
    depthStencilResource_ = CreateDepthStencilResource(device, width, height);

    // DSV作成
    CreateDSV();

    // 深度テクスチャ用SRV作成
    CreateDepthSRV();

    // 初期状態設定
    currentState_ = D3D12_RESOURCE_STATE_DEPTH_WRITE;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DxDepthBuffer::CreateDepthStencilResource(
    Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height) {

    // リソース設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width            = width;
    resourceDesc.Height           = height;
    resourceDesc.MipLevels        = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // ヒープ設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // 深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;
    depthClearValue.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // リソース生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr                                      = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&resource));

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create depth stencil resource heap.\n");
        return nullptr;
    }
    return resource;
}

void DxDepthBuffer::CreateDSV() {
    depthStencilDsvIndex_ = dsvManager_->Allocate();

    // DSV設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    dsvManager_->Create(depthStencilDsvIndex_, depthStencilResource_.Get(), &dsvDesc);
}

void DxDepthBuffer::CreateDepthSRV() {
    depthTextureSrvIndex_ = srvManager_->Allocate();

    depthTextureGPUSrvHandle_ = srvManager_->GetGPUDescriptorHandle(depthTextureSrvIndex_);
    depthTextureCPUSrvHandle_ = srvManager_->GetCPUDescriptorHandle(depthTextureSrvIndex_);

    // SRV設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format                  = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels     = 1;

    srvManager_->CreateForTexture2D(depthTextureSrvIndex_, depthStencilResource_.Get(), srvDesc);
}

void DxDepthBuffer::Clear(ID3D12GraphicsCommandList* commandList) {
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDsvHandle();
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DxDepthBuffer::TransitionState(ID3D12GraphicsCommandList* commandList,const D3D12_RESOURCE_STATES& newState) {
    if (currentState_ != newState) {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = depthStencilResource_.Get();
        barrier.Transition.StateBefore = currentState_;
        barrier.Transition.StateAfter  = newState;

        commandList->ResourceBarrier(1, &barrier);
        currentState_ = newState;
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE DxDepthBuffer::GetDsvHandle() const {
    return dsvManager_->GetCPUDescriptorHandle(depthStencilDsvIndex_);
}

void DxDepthBuffer::Finalize() {
    depthStencilResource_.Reset();
}
