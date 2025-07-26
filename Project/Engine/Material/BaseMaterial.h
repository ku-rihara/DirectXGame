#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

class DirectXCommon;

class BaseMaterial {
public:
    BaseMaterial()          = default;
    virtual ~BaseMaterial() = default;

    // 仮想関数として定義
    virtual void CreateMaterialResource(DirectXCommon* dxCommon)        = 0;
    virtual void UpdateMaterialData(const Vector4& Color)               = 0;
    virtual void SetCommandList(ID3D12GraphicsCommandList* commandList) = 0;
    virtual void DebugImGui()                                           = 0;

protected:
   

    // 共通リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

public:
    // 共通のsetter
    virtual void SetShininess(const float& shininess)                           = 0;
    virtual void SetEnvironmentCoefficient(const float& environmentCoefficient) = 0;
};