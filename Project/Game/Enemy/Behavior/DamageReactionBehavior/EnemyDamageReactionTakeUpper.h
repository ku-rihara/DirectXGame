#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionData.h"
#include "Rendition/EnemyDamageRendition.h"
#include "Vector3.h"
#include <functional>

class PlayerAttackCollider;

/// <summary>
/// TakeUpper ダメージリアクション
/// </summary>
class EnemyDamageReactionTakeUpper : public BaseEnemyDamageReaction {
public:
    EnemyDamageReactionTakeUpper(
        BaseEnemy* boss,
        EnemyDamageReactionData* reactionData,
        const PlayerAttackCollider* playerCollisionInfo);
    ~EnemyDamageReactionTakeUpper() override;

    // 更新、デバッグ
    void Update(float deltaTime) override;
    void Debug() override;

    // 打ち上げから起き上がりが完了するまでは新しい攻撃で割り込まれない
    bool CanBeInterruptedByNewHit() const override { return false; }

private:
    // リアクション演出初期化
    void InitReaction();

    // フェーズ関数
    void UpdatePhase();
    void GetUpPhase();
    void EndPhase();

    // 更新処理
    void UpdateNormal();
    void UpdateTakeUpper();
    void UpdateBounce(float basePosY, float gravity);
    void RotationUpdate();

    // リアクション終了判定・処理
    bool IsReactionFinished() const;
    void OnReactionEnd();

    // アニメーション再生ヘルパー
    void PlayDamageAnim(const std::string& animName, int enemyType);
    void PlayDefaultDamageAnim(int enemyType);

private:
    EnemyDamageReactionData* pReactionData_            = nullptr;
    const PlayerAttackCollider* pPlayerCollisionInfo_  = nullptr;

    // フェーズ管理
    std::function<void()> currentPhase_;

    // タイマー
    float reactionTimer_ = 0.0f;

    // 物理パラメータ
    float blowYPower_;
    float knockBackPower_;

    // 水平ノックバック
    Vector3 knockBackVelocity_;
    float knockBackTimer_ = 0.0f;

    // バウンドパラメータ
    int32_t currentBoundCount_ = 0;
    int32_t maxBoundCount_     = 0;
    float bounceSpeed_;
    float bounceDamping_;
    float initialBounceRate_;
    bool hasReachedGround_ = false;

    // TakeUpper固有パラメータ
    float floatingTime_   = 0.0f;
    float jumpSpeed_      = 0.0f;
    Vector3 initialPosition_;
    bool hasReachedPeak_  = false;
    float floatingTimer_  = 0.0f;
    float takeUpperGravity_;
    Vector3 takeUpperRotateSpeed_;
    float takeUpperFallLimit_;

    // 演出管理
    EnemyDamageRendition damageRendition_;
    bool hasPlayedRendition_ = false;

    // GetUpフェーズ管理
    bool getUpFinished_ = false;

public:
    Vector3 GetKnockBackVelocity() const { return knockBackVelocity_; }
};
