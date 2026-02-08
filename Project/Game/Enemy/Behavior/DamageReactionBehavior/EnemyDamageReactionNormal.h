#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
#include "Rendition/EnemyDamageRendition.h"
#include "Vector3.h"

class PlayerAttackCollisionBox;

/// <summary>
/// Normal ダメージリアクション
/// ノックバックのみ
/// </summary>
class EnemyDamageReactionNormal : public BaseEnemyDamageReaction {
public:
    EnemyDamageReactionNormal(
        BaseEnemy* boss,
        EnemyDamageReactionData* reactionData,
        const PlayerAttackCollisionBox* playerCollisionInfo);
    ~EnemyDamageReactionNormal() override;

    void Update(float deltaTime) override;
    void Debug() override;

private:
    void InitReaction();
    void UpdateNormal();
    bool IsReactionFinished() const;
    void OnReactionEnd();

private:
    EnemyDamageReactionData* pReactionData_ = nullptr;
    const PlayerAttackCollisionBox* pPlayerCollisionInfo_ = nullptr;

    // タイマー
    float reactionTimer_ = 0.0f;
    float totalReactionTime_ = 1.0f;

    // Normal用パラメータ
    Vector3 knockBackVelocity_;
    float knockBackTimer_ = 0.0f;
    float knockBackPower_;

    // 演出管理
    EnemyDamageRendition damageRendition_;
    bool hasPlayedRendition_ = false;

public:
    Vector3 GetKnockBackVelocity() const { return knockBackVelocity_; }
};
