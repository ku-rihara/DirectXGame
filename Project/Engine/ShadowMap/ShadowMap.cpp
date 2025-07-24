#include "ShadowMap.h"
#include "base/SrvManager.h"
#include "Dx/DirectXCommon.h"
#include"Dx/DxDepthBuffer.h"
#include "Lighrt/DirectionalLight.h"
#include "Lighrt/Light.h"

void ShadowMap::Init(DirectXCommon* dxCommon) {
    dxCommon_   = dxCommon;
    srvManager_ = SrvManager::GetInstance();
    CreateConstantBuffer();
    CreateSRVHandle();
    lightDistance_ = 50.0f;
}

void ShadowMap::CreateConstantBuffer() {
    D3D12_RANGE readRange = {};
    HRESULT hr;

    vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(ShadowTransformData));
    hr              = vertexResource_->Map(0, &readRange, reinterpret_cast<void**>(&transformData_));
    if (FAILED(hr)) {
        // エラー処理
        OutputDebugStringA("ConstBuffer Map failed.\n");
    }

    // 値初期化
    transformData_->lightCamera    = MakeIdentity4x4();
    transformData_->cameraPosition = Vector3::ZeroVector();
    transformData_->lightDirection = Vector4::ZeroVector();
}

void ShadowMap::CreateSRVHandle() {
    uint32_t depthIndex = srvManager_->Allocate();

    depthTextureGPUSrvHandle_ = srvManager_->GetGPUDescriptorHandle(depthIndex);
    depthTextureCPUSrvHandle_ = srvManager_->GetCPUDescriptorHandle(depthIndex);

    D3D12_SHADER_RESOURCE_VIEW_DESC depthTextureSrvDesc{};
    depthTextureSrvDesc.Format                  = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    depthTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    depthTextureSrvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
    depthTextureSrvDesc.Texture2D.MipLevels     = 1;

    srvManager_->CreateSRVforTexture2D(depthIndex, dxCommon_->GetDepthBuffer()->GetDepthStencilResource(), depthTextureSrvDesc);
}

void ShadowMap::UpdateLightMatrix() {
    // 位置と向き
    Vector3 lightDir = Light::GetInstance()->GetDirectionalLight()->GetDirection();
    Vector3 lightPos = Vector3(0, lightDistance_, 0) - lightDir * lightDistance_;

    // ライトのビュー行列
    Matrix4x4 lightViewMatrix       = MakeRootAtMatrix(lightPos, Vector3::ZeroVector(), Vector3::ToUp());
    Matrix4x4 lightProjectionMatrix = MakeOrthographicMatrix(-40.0f, 40.0f, -40.0f, 40.0f, 1.0f, 100.0f);

    transformData_->cameraPosition = Light::GetInstance()->GetWorldCameraPos();
    transformData_->lightCamera    = lightViewMatrix * lightProjectionMatrix;
    transformData_->lightDirection = Vector4(lightDir.x, lightDir.y, lightDir.z, 0.0f);
}
