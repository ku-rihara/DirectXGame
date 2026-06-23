#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionData.h"
#include "Rendition/EnemyDamageRendition.h"
#include "Vector3.h"

class PlayerAttackCollider;

/// <summary>
/// Normal ダメージリアクション
/// </summary>
class EnemyDamageReactionNormal : public BaseEnemyDamageReaction {
public:
    EnemyDamageReactionNormal(
        BaseEnemy* boss,
        EnemyDamageReactionData* reactionData,
        const PlayerAttackCollider* playerCollisionInfo,
        bool skipAnimation = false);

    ~EnemyDamageReactionNormal() override;
    
    // 更新、デバッグ
    void Update(float deltaTime) override;
    void Debug() override;

private:
    // リアクション演出初期化、更新
    void InitReaction();
    void UpdateReaction();

    // アニメーション再生ヘルパー
    void PlayDamageAnim(const std::string& animName, int enemyType);
    void PlayDefaultDamageAnim(int enemyType);

    // リアクション終了判定・処理
    bool IsReactionFinished() const;
    void OnReactionEnd();

private:
    EnemyDamageReactionData* pReactionData_           = nullptr;
    const PlayerAttackCollider* pPlayerCollisionInfo_ = nullptr;

    // タイマー
    float reactionTimer_     = 0.0f;
    float totalReactionTime_ = 1.0f;

    // Normal用パラメータ
    Vector3 knockBackVelocity_;
    float knockBackTimer_ = 0.0f;
    float knockBackPower_;

    // 演出管理
    EnemyDamageRendition damageRendition_;
    bool hasPlayedRendition_ = false;

    bool skipAnimation_ = false;

    const float kDefaultKnockBackTime    = 0.5f;
    const float kDefaultKnockBackDamping = 5.0f;

public:
    Vector3 GetKnockBackVelocity() const { return knockBackVelocity_; }
};
