#pragma once
#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
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
/// 次の候補が X のみ・Y のみ・両方かに応じてスプライトを切り替える。
/// </summary>
class NextAttackHintUI {
public:
    NextAttackHintUI()  = default;
    ~NextAttackHintUI() = default;

    /// <summary>
    /// 初期化。スプライト生成と GlobalParameter バインドを行う。
    /// ViewProjection は Player::SetViewProjection() 後に SetViewProjection() で設定する。
    /// </summary>
    void Init(Player* player);

    /// <summary>
    /// ViewProjection ポインタを設定する（Player::SetViewProjection() 呼び出し時に設定）
    /// </summary>
    void SetViewProjection(const KetaEngine::ViewProjection* viewProjection) {
        pViewProjection_ = viewProjection;
    }

    /// <summary>
    /// 毎フレーム更新。コンボ候補を取得しスプライト位置・表示状態を更新する。
    /// </summary>
    void Update();

    /// <summary>
    /// ImGui によるパラメータ調整UI（_DEBUG ビルドのみ有効）
    /// </summary>
    void AdjustParam();

private:
    void RegisterParams();

    /// <summary>
    /// スプライトの位置・スケール・アルファを設定し、登場瞬間にアニメーションを再生する
    /// </summary>
    void ApplySprite(KetaEngine::Sprite* sprite, const Vector2& pos, bool& prevVisible);

private:
    Player*                           pPlayer_         = nullptr;
    const KetaEngine::ViewProjection* pViewProjection_ = nullptr;

    std::unique_ptr<KetaEngine::Sprite> xSprite_;
    std::unique_ptr<KetaEngine::Sprite> ySprite_;

    // --- GlobalParameter で調整可能なパラメータ ---

    /// プレイヤーワールド原点からのオフセット（3D空間）
    Vector3 worldOffset_ = {0.0f, 2.5f, 0.0f};

    /// 各スプライトのサイズ（ピクセル）
    Vector2 spriteScale_ = {48.0f, 48.0f};

    /// X/Y 両方表示時の X ボタンスクリーンローカルオフセット
    Vector2 xLocalOffset_ = {-30.0f, 0.0f};

    /// X/Y 両方表示時の Y ボタンスクリーンローカルオフセット
    Vector2 yLocalOffset_ = {30.0f, 0.0f};

    /// 不透明度
    float alpha_ = 1.0f;

    // --- スポーン時イージングアニメーション ---
    std::string spawnAnimName_     = "NextAttackHintSpawn";
    std::string spawnAnimCategory_ = "NextAttackHintUI";

    /// 前フレームの表示状態（登場瞬間の検出用）
    bool prevXVisible_ = false;
    bool prevYVisible_ = false;

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string            groupName_       = "NextAttackHintUI";
};
