#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
#include "Rendition/EnemyDamageRendition.h"
#include "Vector3.h"
#include <functional>

class PlayerAttackCollisionBox;

/// <summary>
/// Slammed ダメージリアクション
/// 叩きつけ + バウンド
/// </summary>
class EnemyDamageReactionSlammed : public BaseEnemyDamageReaction {
public:
    // 終了時の遷移先
    enum class EndType {
        BackToRoot,
        Death,
    };

public:
    EnemyDamageReactionSlammed(
        BaseEnemy* boss,
        EnemyDamageReactionData* reactionData,
        const PlayerAttackCollisionBox* playerCollisionInfo);
    ~EnemyDamageReactionSlammed() override;

    void Update(float deltaTime) override;
    void Debug() override;

private:
    void InitReaction();

    // フェーズ関数
    void UpdatePhase();
    void EndPhase();

    // 更新処理
    void UpdateNormal();
    void UpdateSlammed();
    void UpdateBounce(float basePosY, float gravity);
    bool IsReactionFinished() const;
    void OnReactionEnd();
    void RotationUpdate();

private:
    EnemyDamageReactionData* pReactionData_ = nullptr;
    const PlayerAttackCollisionBox* pPlayerCollisionInfo_ = nullptr;

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

    // Slammed固有パラメータ
    float slammedGravity_;
    Vector3 slammedRotateSpeed_;

    // 演出管理
    EnemyDamageRendition damageRendition_;
    bool hasPlayedRendition_ = false;

public:
    Vector3 GetKnockBackVelocity() const { return knockBackVelocity_; }
};
