#pragma once
#include "BaseMaterial.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

namespace KetaEngine {

class DirectXCommon;

/// <summary>
/// パーティクル用マテリアルクラス
/// </summary>
class ParticleMaterial : public BaseMaterial {
public:
    ParticleMaterial()  = default;
    ~ParticleMaterial() = default;

    /// <summary>
    /// マテリアルリソースの作成
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon) override;

    /// <summary>
    /// マテリアルデータの更新
    /// </summary>
    /// <param name="Color">色</param>
    void UpdateMaterialData(const Vector4& Color) override;

    /// <summary>
    /// コマンドリストへの設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void SetCommandList(ID3D12GraphicsCommandList* commandList) override;

    void DebugImGui() override; //< ImGuiデバッグ

private:
    struct MaterialData {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvMatrix;
        float shininess;
        float environmentCoefficient;
        float dissolveThreshold;
        float dissolveEdgeWidth;
        Vector3 dissolveEdgeColor;
        int32_t enableDissolve;
    };

private:
    MaterialData* materialData_ = nullptr;

public:
    MaterialData* GetMaterialData() { return materialData_; }
};

}; // KetaEngine
