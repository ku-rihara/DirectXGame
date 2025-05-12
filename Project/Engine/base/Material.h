#pragma once
#include "Matrix4x4.h"
#include "Vector4.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

class DirectXCommon;

class Material {
public:
    enum LightGroup {
        NONE,
        LAMBERT,
        HALF_LAMBERT,
        SPECULAR_REFLECTION,
        POINTLIGHT,
        SOPTLIGHT,
    };

private:
    struct MaterialStructure {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvMatrix;
        float shininess;
        float environmentCoefficient;
    };

public:
    // GPUに送るマテリアルデータの実体
    MaterialStructure* materialData_;

private:
    // GPUリソースへのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

public:
    // コンストラクタ
    Material();

    // マテリアルのリソースを作成する関数
    void CreateMaterialResource(DirectXCommon* dxCommon);

    // マテリアルのデータを更新する関数
    void UpdateMaterialData(const Vector4& Color);

    // シェーダーにデータを送る関数
    void SetCommandList(ID3D12GraphicsCommandList* commandList);

    void DebugImGui();

public:
    void SetShininess(const float& shiniess) { materialData_->shininess = shiniess; }
    void SetEnvironmentCoefficient(const float& environmentCoefficient) { materialData_->environmentCoefficient = environmentCoefficient; }
};
