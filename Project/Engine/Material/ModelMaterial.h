#pragma once
#include "BaseMaterial.h"
#include "Editor/DissolveEditor/DissolvePlayer.h"
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
class DissolvePlayer;

/// <summary>
/// モデル用マテリアルクラス
/// </summary>
class ModelMaterial : public BaseMaterial {
public:
    ModelMaterial()  = default;
    ~ModelMaterial() = default;

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

    /// <summary>
    /// ディゾルブ用ノイズテクスチャの設定
    /// </summary>
    /// <param name="name">テクスチャ名</param>
    void SetDissolveNoizeTexture(const std::string& name);

    void DebugImGui() override; //< ImGuiデバッグ

    // ディゾルブ更新、再生
    void UpdateDissolve(float speedRate = 1.0f);
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

private:
    MaterialData* materialData_                     = nullptr;
    std::unique_ptr<DissolvePlayer> dissolvePlayer_ = nullptr;
    int32_t dissolveTextureIndex_;

public:
    // getter
    MaterialData* GetMaterialData() const { return materialData_; }
    bool IsDissolveFinished() const;
    bool IsDissolvePlaying() const;
};

}; // KetaEngine