#pragma once

#include "ShadowMapData.h"
#include "Matrix4x4.h"
#include <d3d12.h>
#include <wrl/client.h>

class DirectXCommon;
class ShadowMap {
public:

    ShadowMap() = default;
    ~ShadowMap() = default;

    void Init(DirectXCommon* dxCommon);
    void CreateConstantBuffer();

private:
    DirectXCommon* dxCommon_;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    ShadowVertexData* vertexData_;

public:
    ID3D12Resource* GetVertexResource() const { return vertexResource_.Get(); }
    void SetVertexMatrix(Matrix4x4 matrix) { vertexData_->shadowMatrix=matrix; }
};