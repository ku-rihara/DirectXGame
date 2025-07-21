#pragma once
#include "Matrix4x4.h"
#include "Vector4.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

enum class BlendMode {
    None,
    Add,
    Multiply,
    Subtractive,
    Screen,
};

class DirectXCommon;
class Material {
public:
    // コンストラクタ
    Material()  = default;
    ~Material() = default;

    // マテリアルのリソースを作成する関数
    void CreateMaterialResource(DirectXCommon* dxCommon);

    // マテリアルのデータを更新する関数
    void UpdateMaterialData(const Vector4& Color);

    // シェーダーにデータを送る関数
    void SetCommandList(ID3D12GraphicsCommandList* commandList);

    void DebugImGui();

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

private:
    // GPUリソースへのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

public:
    void SetShininess(const float& shiniess) { materialData_->shininess = shiniess; }
    void SetEnvironmentCoefficient(const float& environmentCoefficient) { materialData_->environmentCoefficient = environmentCoefficient; }
};
