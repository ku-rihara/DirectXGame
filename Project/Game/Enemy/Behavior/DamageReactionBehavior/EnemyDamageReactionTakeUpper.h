#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
#include "Rendition/EnemyDamageRendition.h"
#include "Vector3.h"
#include <functional>

class PlayerAttackCollider;

/// <summary>
/// TakeUpper ダメージリアクション
/// 打ち上げ → 浮遊 → 落下 → バウンドの流れ
/// </summary>
class EnemyDamageReactionTakeUpper : public BaseEnemyDamageReaction {
public:
    // 終了時の遷移先
    enum class EndType {
        BackToRoot,
        Death,
    };

public:
    EnemyDamageReactionTakeUpper(
        BaseEnemy* boss,
        EnemyDamageReactionData* reactionData,
        const PlayerAttackCollider* playerCollisionInfo);
    ~EnemyDamageReactionTakeUpper() override;

    void Update(float deltaTime) override;
    void Debug() override;

private:
    void InitReaction();

    // フェーズ関数
    void UpdatePhase();
    void GetUpPhase();
    void EndPhase();

    // 更新処理
    void UpdateNormal();
    void UpdateTakeUpper();
    void UpdateBounce(float basePosY, float gravity);
    bool IsReactionFinished() const;
    void OnReactionEnd();
    void RotationUpdate();

private:
    EnemyDamageReactionData* pReactionData_ = nullptr;
    const PlayerAttackCollider* pPlayerCollisionInfo_ = nullptr;

    // フェーズ管理
    std::function<void()> currentPhase_;
    EndType endType_ = EndType::BackToRoot;

    // タイマー
    float reactionTimer_ = 0.0f;

    // 物理パラメータ
    float blowYPower_;
    float knockBackPower_;

    // Normal用パラメータ
    Vector3 knockBackVelocity_;
    float knockBackTimer_ = 0.0f;

    // バウンドパラメータ
    int32_t currentBoundCount_ = 0;
    int32_t maxBoundCount_ = 0;
    float bounceSpeed_;
    float bounceDamping_;
    float initialBounceRate_;
    bool hasReachedGround_ = false;

    // Upperパラメータ
    float floatingTime_ = 0.0f;
    float jumpSpeed_ = 0.0f;
    Vector3 initialPosition_;
    bool hasReachedPeak_ = false;
    float floatingTimer_ = 0.0f;
    float takeUpperGravity_;
    Vector3 takeUpperRotateSpeed_;
    float takeUpperFallLimit_;

    // 演出管理
    EnemyDamageRendition damageRendition_;
    bool hasPlayedRendition_ = false;

    // GetUpフェーズ管理
    bool getUpFinished_ = false;

    // バウンド死亡フラグ（1バウンド後にアニメなしで死亡）
    bool  isDeathBounce_     = false;
    bool  isDeathBurstPhase_ = false;
    float deathBurstTimer_   = 0.0f;

public:
    Vector3 GetKnockBackVelocity() const { return knockBackVelocity_; }
};
