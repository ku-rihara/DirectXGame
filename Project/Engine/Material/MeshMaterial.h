#pragma once
#include "BaseMaterial.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

class DirectXCommon;

class MeshMaterial : public BaseMaterial {
public:
    // コンストラクタ
    MeshMaterial()  = default;
    ~MeshMaterial() = default;

    // マテリアルのリソースを作成する関数
    void CreateMaterialResource(DirectXCommon* dxCommon) override;

    // マテリアルのデータを更新する関数
    void UpdateMaterialData(const Vector4& Color) override;

    // シェーダーにデータを送る関数
    void SetCommandList(ID3D12GraphicsCommandList* commandList) override;

    void DebugImGui() override;

private:
    struct MaterialData {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvMatrix;
        float shininess;
        float environmentCoefficient;
    };

public:
    // GPUに送るマテリアルデータの実体
    MaterialData* materialData_ = nullptr;

public:
    void SetShininess(const float& shininess) override { materialData_->shininess = shininess; }
    void SetEnvironmentCoefficient(const float& environmentCoefficient) override { materialData_->environmentCoefficient = environmentCoefficient; }
};