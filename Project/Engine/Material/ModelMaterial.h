#pragma once
#include "BaseMaterial.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl.h>

enum class BlendMode {
    None,
    Add,
    Multiply,
    Subtractive,
    Screen,
};

namespace KetaEngine {

class DirectXCommon;
class DissolvePlayer; // 前方宣言

/// <summary>
/// モデル用マテリアルクラス
/// </summary>
class ModelMaterial : public BaseMaterial {
public:
    ModelMaterial();
    ~ModelMaterial();

    /// <summary>
    /// マテリアルリソースの作成
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void CreateMaterialResource(DirectXCommon* dxCommon) override;

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

    /// <summary>
    /// ディゾルブ用ノイズテクスチャの設定
    /// </summary>
    /// <param name="name">テクスチャ名</param>
    void SetDissolveNoizeTexture(const std::string& name);

    void DebugImGui() override; //< ImGuiデバッグ

    // ディゾルブプレイヤー関連
    /// <summary>
    /// ディゾルブ更新
    /// </summary>
    /// <param name="speedRate">速度倍率</param>
    void UpdateDissolve(float speedRate = 1.0f);

    /// <summary>
    /// ディゾルブ再生
    /// </summary>
    /// <param name="dissolveName">ディゾルブ名</param>
    void PlayDissolve(const std::string& dissolveName);


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
    int32_t dissolveTextureIndex_;
    std::unique_ptr<DissolvePlayer> dissolvePlayer_; 

public:
    void SetShininess(float shininess) override { materialData_->shininess = shininess; }
    void SetEnvironmentCoefficient(float environmentCoefficient) override { materialData_->environmentCoefficient = environmentCoefficient; }

    void SetDissolveThreshold(float threshold) { materialData_->dissolveThreshold = threshold; }
    void SetDissolveEdgeColor(const Vector3& color) { materialData_->dissolveEdgeColor = color; }
    void SetDissolveEdgeWidth(float width) { materialData_->dissolveEdgeWidth = width; }
    void SetEnableDissolve(bool enable) { materialData_->enableDissolve = enable ? 1 : 0; }

    float GetDissolveThreshold() const { return materialData_->dissolveThreshold; }
    Vector3 GetDissolveEdgeColor() const { return materialData_->dissolveEdgeColor; }
    float GetDissolveEdgeWidth() const { return materialData_->dissolveEdgeWidth; }
    bool IsDissolveEnabled() const { return materialData_->enableDissolve != 0; }

    // ディゾルブプレイヤー関連のgetter
    bool IsDissolveFinished() const;
    bool IsDissolvePlaying() const;
};

}; // KetaEngine