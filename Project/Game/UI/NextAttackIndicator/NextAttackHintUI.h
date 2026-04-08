#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "UI/ComboAsistUI/AsistParts/ComboAsistButtonUI.h"
#include "Vector2.h"
#include "Vector3.h"
#include <memory>
#include <string>

namespace KetaEngine {
class ViewProjection;
} // namespace KetaEngine

class Player;

/// <summary>
/// 攻撃中に次のコンボ入力候補（X / Y ボタン）をプレイヤー近傍に表示するUIクラス。
/// ComboAsistButtonUI を使い、開放状況の表示と ScaleIn/ScaleOut イージングに対応。
/// </summary>
class NextAttackHintUI {
public:
    NextAttackHintUI()  = default;
    ~NextAttackHintUI() = default;

    /// <summary>
    /// 初期化。ボタンUI生成と GlobalParameter バインドを行う。
    /// </summary>
    void Init(Player* player);

    /// <summary>
    /// ViewProjection ポインタを設定する
    /// </summary>
    void SetViewProjection(const KetaEngine::ViewProjection* viewProjection) {
        pViewProjection_ = viewProjection;
    }

    /// <summary>
    /// 毎フレーム更新
    /// </summary>
    void Update();

    /// <summary>
    /// ImGui によるパラメータ調整UI（_DEBUG ビルドのみ有効）
    /// </summary>
    void AdjustParam();

private:
    void RegisterParams();

private:
    Player*                           pPlayer_         = nullptr;
    const KetaEngine::ViewProjection* pViewProjection_ = nullptr;

    std::unique_ptr<ComboAsistButtonUI> xButtonUI_;
    std::unique_ptr<ComboAsistButtonUI> yButtonUI_;

    // --- GlobalParameter で調整可能なパラメータ ---

    /// プレイヤーワールド原点からのオフセット（3D空間）
    Vector3 worldOffset_ = {0.0f, 2.5f, 0.0f};

    /// 各ボタンUIのスケール
    Vector2 spriteScale_ = {48.0f, 48.0f};

    /// X ボタンのスクリーンローカルオフセット
    Vector2 xLocalOffset_ = {-30.0f, 0.0f};

    /// Y ボタンのスクリーンローカルオフセット
    Vector2 yLocalOffset_ = {30.0f, 0.0f};

    /// ロック UI のスケール（ボタン UI とは独立して調整可能）
    Vector2 lockScale_ = {48.0f, 48.0f};

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string            groupName_       = "NextAttackHintUI";
};
