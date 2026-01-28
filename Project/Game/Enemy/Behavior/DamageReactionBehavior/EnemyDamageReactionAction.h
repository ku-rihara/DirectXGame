#pragma once

// BaseReactionBehavior
#include "BaseEnemyDamageReaction.h"
// ReactionData
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
// EnemyDamageRendition
#include "Rendition/EnemyDamageRendition.h"
// math
#include "Vector3.h"
// std
#include <memory>

class PlayerAttackCollisionBox;
class EnemyDamageRenditionData;

class EnemyDamageReactionAction : public BaseEnemyDamageReaction {
public:
    // リアクション状態
    enum class ReactionState {
        Normal,
        Slammed,
        TakeUpper,
    };

public:
    // コンストラクタ
    EnemyDamageReactionAction(BaseEnemy* boss, EnemyDamageReactionData* reactionData, const PlayerAttackCollisionBox* playerCollisionInfo);
    ~EnemyDamageReactionAction() override;

    void Update(float deltaTime) override;
    void Debug() override;

private:
    // リアクション初期化
    void InitReaction();

    void InitNormalReaction(const EnemyDamageReactionData::NormalParam& param);
    void InitSlammedReaction(
        const EnemyDamageReactionData::BoundParam& boundParam,
        const EnemyDamageReactionData::SlammedParam& param);
    void InitTakeUpperReaction(
        const EnemyDamageReactionData::BoundParam& boundParam,
        const EnemyDamageReactionData::TakeUpperParam& param);

    // 各リアクション処理
    void UpdateNormal();
    void UpdateSlammed();
    void UpdateTakeUpper();

    // バウンド処理
    void UpdateBounce(float basePosY, float gravity, float rotateSpeed);

    // リアクション終了判定
    bool IsReactionFinished() const;

    // リアクション終了時の処理
    void OnReactionEnd();

private:
    EnemyDamageReactionData* pReactionData_          = nullptr;
    const PlayerAttackCollisionBox* pPlayerCollisionInfo_ = nullptr;

    ReactionState currentState_ = ReactionState::Normal;

    // 物理パラメータ
    float blowYPower_;
    float knockBackPower_;

    // タイマー
    float reactionTimer_     = 0.0f;
    float totalReactionTime_ = 1.0f;

    // Normal用パラメータ
    Vector3 knockBackVelocity_;
    float knockBackTimer_ = 0.0f;

    // バウンドパラメータ
    int32_t currentBoundCount_ = 0;
    int32_t maxBoundCount_     = 0;
    float bounceSpeed_;
    float bounceDamping_;
    float initialBounceRate_;
    bool hasReachedGround_ = false;

    // Slammed固有パラメータ
    float slammedGravity_;
    float slammedRotateSpeed_;

    // TakeUpper固有パラメータ
    float floatingTime_ = 0.0f;
    float jumpSpeed_    = 0.0f;
    Vector3 initialPosition_;
    bool hasReachedPeak_ = false;
    float floatingTimer_ = 0.0f;
    float takeUpperGravity_;
    float takeUpperRotateSpeed_;
    float takeUpperFallLimit_;

    // 演出管理
    EnemyDamageRendition damageRendition_;
    int32_t currentRenditionIndex_ = 0;
    bool hasPlayedRendition_       = false;

public:
    Vector3 GetKnockBackVelocity() const { return knockBackVelocity_; }
};