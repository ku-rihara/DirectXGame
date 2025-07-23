#include "ShadowMap.h"
#include"Dx/DirectXCommon.h"

void ShadowMap::Init(DirectXCommon* dxCommon) {
    dxCommon_ = dxCommon;
    CreateConstantBuffer();
}

void ShadowMap::CreateConstantBuffer() {
    D3D12_RANGE readRange = {};
    HRESULT hr;

    vertexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(ShadowVertexData));
    hr              = vertexResource_->Map(0, &readRange, reinterpret_cast<void**>(&vertexData_));
    if (FAILED(hr)) {
        // エラー処理
        OutputDebugStringA("ConstBuffer Map failed.\n");
    }

    vertexData_->shadowMatrix = MakeIdentity4x4();
}