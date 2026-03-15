#pragma once
#include "3d/ViewProjection.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "math/Vector3.h"
#include <string>

namespace KetaEngine {

class ModelMaterial;

/// <summary>
/// ディザオクルージョン
/// プレイヤーを隠しているピクセルだけをスクリーンスペースでディザ抜きする
/// </summary>
class DitherOcclusion {
public:
    DitherOcclusion()  = default;
    ~DitherOcclusion() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    void Init();

    /// <summary>
    /// ディザ抜き対象として登録する
    /// </summary>
    void Add(ModelMaterial* material);

    /// <summary>
    /// 毎フレーム更新
    /// </summary>
    void Update(const ViewProjection& viewProjection, const Vector3& playerWorldPos);

    /// <summary>
    /// ImGuiでパラメータを編集するUI
    /// </summary>
    void AdjustParam();

private:
    void RegisterParams();

private:
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    float screenRadius_                           = 150.0f; // 判定半径
    float ditherAlpha_                            = 0.2f;   // ディザ閾値

    const std::string kGroupName_ = "DitherOcclusion";
};

} // namespace KetaEngine
