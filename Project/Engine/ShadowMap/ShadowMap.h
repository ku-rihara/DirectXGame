#pragma once

#include "ShadowMapData.h"
#include "Matrix4x4.h"
#include <d3d12.h>
#include <wrl/client.h>

class SrvManager;
class DirectXCommon;
class ShadowMap {
public:

    ShadowMap() = default;
    ~ShadowMap() = default;

    void Init(DirectXCommon* dxCommon);
    void CreateConstantBuffer();
    void CreateSRVHandle();
    void UpdateLightMatrix();
    void Draw();

private:
    DirectXCommon* dxCommon_;
    SrvManager* srvManager_;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    ShadowTransformData* transformData_;
    float lightDistance_;

    D3D12_GPU_DESCRIPTOR_HANDLE depthTextureGPUSrvHandle_;
    D3D12_CPU_DESCRIPTOR_HANDLE depthTextureCPUSrvHandle_;

public:
    ID3D12Resource* GetVertexResource() const { return vertexResource_.Get(); }
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return depthTextureGPUSrvHandle_; }
};