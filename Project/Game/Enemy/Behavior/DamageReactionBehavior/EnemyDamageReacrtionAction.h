#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
#include "Vector3.h"

class PlayerCollisionInfo;
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
    EnemyDamageReactionAction(BaseEnemy* boss, EnemyDamageReactionData* reactionData, const PlayerCollisionInfo* playerCollisionInfo);
    ~EnemyDamageReactionAction() override;

    void Update() override;
    void Debug() override;

private:
    // リアクション初期化
    void InitReaction();

    void InitNormalReaction(const EnemyDamageReactionData::NormalParam& param);
    void InitSlammedReaction(const EnemyDamageReactionData::SlammedParam& param);
    void InitTakeUpperReaction(const EnemyDamageReactionData::TakeUpperParam& param);

    // 各リアクション処理
    void UpdateNormal();
    void UpdateSlammed();
    void UpdateTakeUpper();

    // 演出更新
    void UpdateRenditions();

    // リアクション終了判定
    bool IsReactionFinished() const;

    // リアクション終了時の処理
    void OnReactionEnd();

private:
    EnemyDamageReactionData* pReactionData_          = nullptr;
    const PlayerCollisionInfo* pPlayerCollisionInfo_ = nullptr;

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
  
    // Slammed用パラメータ
    int32_t currentBoundCount_ = 0;
    int32_t maxBoundCount_     = 0;
    float bounceSpeed_;
    float bounceDamping_;
    bool hasReachedGround_ = false; // 地面に到達したか

    // TakeUpper用パラメータ
    float floatingTime_ = 0.0f;
    float jumpSpeed_    = 0.0f;
    Vector3 initialPosition_;
    bool hasReachedPeak_ = false; // 頂点に到達したか
    float floatingTimer_ = 0.0f; // 浮遊時間カウント
    bool hasBounced_     = false; // バウンド済みか

    // 演出管理
    int32_t currentRenditionIndex_ = 0;
    bool hasPlayedRendition_       = false;
};