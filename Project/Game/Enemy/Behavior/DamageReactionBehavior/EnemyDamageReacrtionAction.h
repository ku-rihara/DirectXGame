#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Vector3.h"

class PlayerCollisionInfo;
class EnemyDamageReactionData;
class EnemyDamageRenditionData;

class EnemyDamageReactionAction : public BaseEnemyDamageReaction {
public:
    // コンストラクタ
    EnemyDamageReactionAction(BaseEnemy* boss, EnemyDamageReactionData* reactionData, const PlayerCollisionInfo* playerCollisionInfo);
    ~EnemyDamageReactionAction() override;

    void Update() override;
    void Debug() override;

private:
    /// <summary>
    /// リアクションの初期化処理
    /// </summary>
    void InitializeReaction();

    /// <summary>
    /// Normal状態の更新
    /// </summary>
    void UpdateNormal();

    /// <summary>
    /// Slammed状態の更新
    /// </summary>
    void UpdateSlammed();

    /// <summary>
    /// TakeUpper状態の更新
    /// </summary>
    void UpdateTakeUpper();

    /// <summary>
    /// 演出の更新
    /// </summary>
    void UpdateRenditions();

    /// <summary>
    /// リアクション終了判定
    /// </summary>
    /// <returns>終了ならtrue</returns>
    bool IsReactionFinished() const;

private:
    EnemyDamageReactionData* pReactionData_    = nullptr;
    const PlayerCollisionInfo* pPlayerCollisionInfo_ = nullptr;

    // リアクション状態
    enum class ReactionState {
        Normal,
        Slammed,
        TakeUpper,
    };
    ReactionState currentState_ = ReactionState::Normal;

    // 物理パラメータ
    float blowYPower_ = 0.0f; // プレイヤー攻撃から取得したY方向の力

    // タイマー
    float reactionTimer_     = 0.0f;
    float totalReactionTime_ = 1.0f;

    // Slammed用パラメータ
    int32_t currentBoundCount_ = 0;
    int32_t maxBoundCount_     = 0;
    float bounceSpeed_         = 0.0f;
    float bounceDamping_       = 0.5f;
    bool hasReachedGround_     = false; // 地面に到達したか

    // TakeUpper用パラメータ
    float floatingTime_ = 1.0f;
    float jumpSpeed_    = 0.0f;
    Vector3 initialPosition_;
    bool hasReachedPeak_ = false; // 頂点に到達したか
    float floatingTimer_ = 0.0f; // 浮遊時間カウント

    // 演出管理
    int32_t currentRenditionIndex_ = 0;
    bool hasPlayedRendition_       = false;
};