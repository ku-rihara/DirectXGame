#include "ShadowMap.h"
#include "3d/Model.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"
#include "base/DsvManager.h"
#include "base/SrvManager.h"
#include "base/WinApp.h"
#include "Dx/DirectXCommon.h"
#include "Dx/DxDepthBuffer.h"
#include "Lighrt/DirectionalLight.h"
#include "Lighrt/Light.h"
#include "Pipeline/ShadowMapPipeline.h"
#include <imgui.h>

ShadowMap* ShadowMap::GetInstance() {
    static ShadowMap instance;
    return &instance;
}

void ShadowMap::Init(DirectXCommon* dxCommon) {
    dxCommon_   = dxCommon;
    srvManager_ = SrvManager::GetInstance();
    dsvManager_ = DsvManager::GetInstance();
    pipeline_   = std::make_unique<ShadowMapPipeline>();

    shadowMapWidth_  = 2024;
    shadowMapHeight_ = 2024;
    lightDistance_   = 50.0f;
    targetPos_       = Vector3::ZeroVector();

    lightViewProjection_ = std::make_unique<ViewProjection>();
    lightViewProjection_->Init();
    lightViewProjection_->translation_ = Vector3::ZeroVector();
    lightViewProjection_->projectionType_ = ViewProjection::ProjectionType::ORTHOGRAPHIC;

    // パイプライン初期化
    pipeline_->Init(dxCommon_);

    // リソース作成、SRV,DSV
    CreateConstantBuffer();
    CreateShadowMapResource(shadowMapWidth_, shadowMapHeight_);
    CreateSRVHandle();
    CreateDSVHandle();

    // ビューポートとシザー矩形の設定
    shadowMapViewport_.Width    = static_cast<float>(shadowMapWidth_);
    shadowMapViewport_.Height   = static_cast<float>(shadowMapHeight_);
    shadowMapViewport_.TopLeftX = 0.0f;
    shadowMapViewport_.TopLeftY = 0.0f;
    shadowMapViewport_.MinDepth = 0.0f;
    shadowMapViewport_.MaxDepth = 1.0f;

    shadowMapScissorRect_.left   = 0;
    shadowMapScissorRect_.top    = 0;
    shadowMapScissorRect_.right  = static_cast<LONG>(shadowMapWidth_);
    shadowMapScissorRect_.bottom = static_cast<LONG>(shadowMapHeight_);
}

void ShadowMap::CreateConstantBuffer() {
    D3D12_RANGE readRange = {};
    HRESULT hr;

    vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(ShadowTransformData));
    hr              = vertexResource_->Map(0, &readRange, reinterpret_cast<void**>(&transformData_));
    if (FAILED(hr)) {
        OutputDebugStringA("ShadowMap ConstBuffer Map failed.\n");
    }

    // 値初期化
    transformData_->lightCamera = MakeIdentity4x4();
}
void ShadowMap::CreateShadowMapResource(uint32_t width, uint32_t height) {
    // リソース設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Width              = width;
    resourceDesc.Height             = height;
    resourceDesc.DepthOrArraySize   = 1;
    resourceDesc.MipLevels          = 1;
    resourceDesc.Format             = DXGI_FORMAT_R32_TYPELESS;
    resourceDesc.SampleDesc.Count   = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // クリア値設定
    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format               = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth   = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    // ヒープ設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // シャドウマップリソース生成
    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&shadowMapResource_));

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create shadow map resource.\n");
    }

    // 初期状態を記録
    currentShadowMapState_ = D3D12_RESOURCE_STATE_DEPTH_WRITE;
}

void ShadowMap::CreateSRVHandle() {
    // シャドウマップのSRV
    shadowMapSrvIndex_     = srvManager_->Allocate();
    shadowMapSrvGPUHandle_ = srvManager_->GetGPUDescriptorHandle(shadowMapSrvIndex_);
    shadowMapSrvCPUHandle_ = srvManager_->GetCPUDescriptorHandle(shadowMapSrvIndex_);

    D3D12_SHADER_RESOURCE_VIEW_DESC shadowMapSrvDesc{};
    shadowMapSrvDesc.Format                  = DXGI_FORMAT_R32_FLOAT;
    shadowMapSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    shadowMapSrvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    shadowMapSrvDesc.Texture2D.MipLevels     = 1;

    srvManager_->CreateSRVforTexture2D(shadowMapSrvIndex_, shadowMapResource_.Get(), shadowMapSrvDesc);
}

void ShadowMap::CreateDSVHandle() {
    shadowMapDsvIndex_  = dsvManager_->Allocate();
    shadowMapDsvHandle_ = dsvManager_->GetCPUDescriptorHandle(shadowMapDsvIndex_);

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format        = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    dsvManager_->CreateDSV(shadowMapDsvIndex_, shadowMapResource_.Get(), &dsvDesc);
}

void ShadowMap::UpdateLightMatrix() {
    // ライト情報の取得と位置計算
    Vector3 lightDirection = GetLightDirectionAndPosition();

    // ライトの向きを設定
    SetLightOrientation(lightDirection);

    // ViewProjectionの行列を更新
    lightViewProjection_->UpdateMatrix();
    transformData_->lightCamera = lightViewProjection_->matView_ * lightViewProjection_->matProjection_;
}

Vector3 ShadowMap::GetLightDirectionAndPosition() {
    // ライトの方向ベクトルを取得
    Vector3 lightDirection = Light::GetInstance()->GetDirectionalLight()->GetDirection();
    lightDirection         = lightDirection.Normalize();
    lightDirection_        = Vector4(lightDirection.x, lightDirection.y, lightDirection.z, 0.0f);

    // ライト位置を計算して設定
    lightViewProjection_->translation_ = targetPos_ - lightDirection * lightDistance_;

    return lightDirection;
}

void ShadowMap::TransitionResourceState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState) {
    if (currentShadowMapState_ != newState) {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = shadowMapResource_.Get();
        barrier.Transition.StateBefore = currentShadowMapState_;
        barrier.Transition.StateAfter  = newState;

        commandList->ResourceBarrier(1, &barrier);
        currentShadowMapState_ = newState;
    }
}

void ShadowMap::SetLightOrientation(const Vector3& lightDirection) {
    // ライトがターゲットを向くための回転を計算
    Vector3 forward = (targetPos_ - lightViewProjection_->translation_).Normalize();

    lightDirection;
    lightViewProjection_->rotation_.y = atan2f(forward.x, forward.z);
    lightViewProjection_->rotation_.x = -asinf(forward.y);
    lightViewProjection_->rotation_.z = 0.0f;
}


void ShadowMap::PreDraw() {
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // シャドウマップ専用のパイプラインを設定
    pipeline_->PreDraw(commandList);

    // シャドウマップリソースの状態遷移
    TransitionResourceState(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    // レンダーターゲットの設定
    commandList->OMSetRenderTargets(0, nullptr, FALSE, &shadowMapDsvHandle_);

    // 深度バッファのクリア
    commandList->ClearDepthStencilView(shadowMapDsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // ビューポートとシザー矩形の設定
    commandList->RSSetViewports(1, &shadowMapViewport_);
    commandList->RSSetScissorRects(1, &shadowMapScissorRect_);
}

void ShadowMap::PostDraw() {
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // シャドウマップリソースの状態遷移
    TransitionResourceState(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ShadowMap::DebugImGui() {
    if (ImGui::TreeNode("ShadowMap Debug")) {
        ImGui::DragFloat3("targetPos", &targetPos_.x,0.5f);
        ImGui::DragFloat("lightDistance", &lightDistance_, 1.0f, 1.0f, 100.0f);

        ImGui::TreePop();
    }
}

void ShadowMap::Finalize() {
    if (vertexResource_) {
        vertexResource_->Unmap(0, nullptr);
        vertexResource_.Reset();
    }

    if (shadowMapResource_) {
        shadowMapResource_.Reset();
    }
    if (pipeline_) {
        pipeline_.reset();
    }
}