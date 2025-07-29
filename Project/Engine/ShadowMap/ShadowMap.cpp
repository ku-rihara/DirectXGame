#include "ShadowMap.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "base/DsvManager.h"
#include "base/RtvManager.h"
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

    shadowMapWidth_  = 2048;
    shadowMapHeight_ = 2048;
    lightDistance_   = 1.0f;

    // パイプライン初期化
    pipeline_->Init(dxCommon_);

    // リソース作成、SRV,DSV
    CreateConstantBuffer();
    CreateShadowMapResource(shadowMapWidth_, shadowMapHeight_);
    CreateSRVHandle();
    CreateRTVHandle();
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
    transformData_->lightCamera    = MakeIdentity4x4();
    transformData_->cameraPosition = Vector3::ZeroVector();
    transformData_->lightDirection = Vector4::ZeroVector();

    // world
    worldMatrixResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(WorldMatrixData));
    hr                   = worldMatrixResource_->Map(0, &readRange, reinterpret_cast<void**>(&worldMatrixData_));
    if (FAILED(hr)) {
        OutputDebugStringA("ShadowMap WorldMatrix Map failed.\n");
    }

    // 初期化
    worldMatrixData_->world_ = MakeIdentity4x4();
}
// ShadowMap.cpp の CreateShadowMapResource() を更新
void ShadowMap::CreateShadowMapResource(uint32_t width, uint32_t height) {
    // ★デバッグ用にカラーバッファも作成
    // カラーバッファリソース作成
    D3D12_RESOURCE_DESC colorResourceDesc{};
    colorResourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    colorResourceDesc.Width              = width;
    colorResourceDesc.Height             = height;
    colorResourceDesc.DepthOrArraySize   = 1;
    colorResourceDesc.MipLevels          = 1;
    colorResourceDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    colorResourceDesc.SampleDesc.Count   = 1;
    colorResourceDesc.SampleDesc.Quality = 0;
    colorResourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    colorResourceDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE colorClearValue{};
    colorClearValue.Format   = DXGI_FORMAT_R8G8B8A8_UNORM;
    colorClearValue.Color[0] = 0.0f; // R
    colorClearValue.Color[1] = 0.0f; // G
    colorClearValue.Color[2] = 0.0f; // B
    colorClearValue.Color[3] = 1.0f; // A

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    // ★カラーバッファリソース生成
    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &colorResourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &colorClearValue,
        IID_PPV_ARGS(&shadowMapColorResource_));

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create shadow map color resource.\n");
    }

    // デプスバッファのみでシャドウマップを実装
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

    // シャドウマップリソース生成
    hr = dxCommon_->GetDevice()->CreateCommittedResource(
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
    /* Light::GetInstance()->GetWorldCameraPos()*/
    // カメラの設定値
    Vector3 eye(0.0f, 20.0f, -25.0f);
    Vector3 target(0.0f, 10.0f, 0.0f);
    Vector3 up(0.0f, 1.0f, 0.0f);

    // カメラ位置を設定
    transformData_->cameraPosition = eye;

    // ライトの方向ベクトル
    Vector3 lightDirection         = Light::GetInstance()->GetDirectionalLight()->GetDirection();
    lightDirection                 = (lightDirection).Normalize();
    transformData_->lightDirection = Vector4(lightDirection.x, lightDirection.y, lightDirection.z, 0.0f);

    // カメラアーム
    Vector3 armVector = target - eye;
    float armLength   = (armVector).Length();

    // ライト位置を計算
    Vector3 lightPos = target + lightDirection * armLength;

    // ライトカメラのビュー行列を作成
    Matrix4x4 lightView = MakeRootAtMatrix(lightPos, target, up);

    // 正射影行列を作成
    Matrix4x4 lightProjection = MakeOrthographicMatrix(
        -20.0f, 20.0f,
        20.0f, -20.0f,
        1.0f, 100.0f);

    // ライトカメラ行列
    transformData_->lightCamera = lightView * lightProjection;
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
void ShadowMap::CreateRTVHandle() {

    shadowMapRtvIndex_  = RtvManager::GetInstance()->Allocate();
    shadowMapRtvHandle_ = RtvManager::GetInstance()->GetCPUDescriptorHandle(shadowMapRtvIndex_);

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    RtvManager::GetInstance()->CreateRTV(shadowMapRtvIndex_, shadowMapColorResource_.Get(), &rtvDesc);
}
void ShadowMap::PreDraw() {
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // シャドウマップ専用のパイプラインを設定
    pipeline_->PreDraw(commandList);

    // シャドウマップリソースの状態遷移
    TransitionResourceState(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    // ★デバッグ用：カラーバッファとデプスバッファの両方を設定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = shadowMapRtvHandle_;
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &shadowMapDsvHandle_);

    // ★カラーバッファとデプスバッファをクリア
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
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
        ImGui::DragFloat("Light Distance", &lightDistance_, 1.0f, 1.0f, 100.0f);

        Vector3 lightDir = Light::GetInstance()->GetDirectionalLight()->GetDirection();
        ImGui::Text("Light Direction: (%.2f, %.2f, %.2f)", lightDir.x, lightDir.y, lightDir.z);

        Vector3 lightPos = Vector3::ZeroVector() - lightDir * lightDistance_;
        ImGui::Text("Light Position: (%.2f, %.2f, %.2f)", lightPos.x, lightPos.y, lightPos.z);

        // 投影行列のパラメータ調整
        static float orthoSize = 40.0f;
        static float nearPlane = 1.0f;
        static float farPlane  = 100.0f;

        if (ImGui::DragFloat("Ortho Size", &orthoSize, 1.0f, 5.0f, 100.0f)) {
            // 投影行列を再計算
        }

        ImGui::TreePop();
    }
}

void ShadowMap::Finalize() {
    if (vertexResource_) {
        vertexResource_->Unmap(0, nullptr);
        vertexResource_.Reset();
    }
    if (worldMatrixResource_) {
        worldMatrixResource_->Unmap(0, nullptr);
        worldMatrixResource_.Reset();
    }
    if (shadowMapResource_) {
        shadowMapResource_.Reset();
    }
    if (pipeline_) {
        pipeline_.reset();
    }
}