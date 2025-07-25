#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
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

    void SetDissolveNoizeTexture(const std::string&name);

    void DebugImGui();

private:
    struct MaterialData {
        Vector4 color;
        int32_t enableLighting;
        float padding1[3]; 
        Matrix4x4 uvTransform; 
        float shininess;
        float environmentCoefficient;
        float dissolveThreshold;
        float dissolveEdgeWidth;
        Vector3 dissolveEdgeColor; 
        int32_t enableDissolve;
    };

public:

    MaterialData* materialData_ = nullptr;
private:
    // GPUリソースへのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    int32_t dissolveTextureIndex_;

public:
   
    void SetShininess(const float& shininess) { materialData_->shininess = shininess; }
    void SetEnvironmentCoefficient(const float& environmentCoefficient) { materialData_->environmentCoefficient = environmentCoefficient; }

    // Dissolve
    void SetDissolveThreshold(const float& threshold) { materialData_->dissolveThreshold = threshold; }
    void SetDissolveEdgeColor(const Vector3& color) { materialData_->dissolveEdgeColor = color; }
    void SetDissolveEdgeWidth(const float& width) { materialData_->dissolveEdgeWidth = width; }
    void SetEnableDissolve(bool enable) { materialData_->enableDissolve = enable ? 1 : 0; }

    float GetDissolveThreshold() const { return materialData_->dissolveThreshold; }
    Vector3 GetDissolveEdgeColor() const { return materialData_->dissolveEdgeColor; }
    float GetDissolveEdgeWidth() const { return materialData_->dissolveEdgeWidth; }
    bool IsDissolveEnabled() const { return materialData_->enableDissolve != 0; }
};