/// behavior
#include "EnemyDeath.h"
/// obj
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// math
#include "Frame/Frame.h"
#include "MathFunction.h"

// 初期化
EnemyDeath::EnemyDeath(BaseEnemy* boss)
    : BaseEnemyDamageReaction("EnemyDeath", boss) {

    step_ = Step::DIRECTIONSET;
}

void EnemyDeath::AngleCaluclation() {
    Vector3 playerPos = pBaseEnemy_->GetPlayer()->GetWorldPosition();

    direction_   = pBaseEnemy_->GetDirectionToTarget(playerPos);
    direction_.y = 0.0f;
    direction_   = direction_.Normalize();

    objectiveAngle_ = std::atan2(direction_.x, direction_.z);
}

EnemyDeath::~EnemyDeath() {
}

void EnemyDeath::Update([[maybe_unused]] float deltaTime) {
    switch (step_) {
    /// ------------------------------------------------------
    /// 方向計算 → アニメーションへ
    ///---------------------------------------------------------
    case Step::DIRECTIONSET:
        AngleCaluclation();
        pBaseEnemy_->SetRotationY(objectiveAngle_);
        step_ = Step::ANIMATION;
        break;

    /// -------------------------------------------------------
    /// 死亡アニメーション再生・終了待ち
    ///---------------------------------------------------------
    case Step::ANIMATION:
        if (!deathAnimStarted_) {
            deathAnimStarted_ = true;
            const std::string& deathAnim = pBaseEnemy_->GetAnimationName(BaseEnemy::AnimationType::Death);
            if (!deathAnim.empty()) {
                pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Death, false);
                pBaseEnemy_->GetAnimationObject()->SetAnimationEndCallback(deathAnim, [this]() {
                    deathAnimFinished_ = true;
                });
            } else {
                // アニメーション未設定の場合はすぐに次へ
                deathAnimFinished_ = true;
            }
        }
        if (deathAnimFinished_) {
            step_ = Step::BURST;
        }
        break;

    /// -------------------------------------------------------
    /// 爆散
    ///---------------------------------------------------------
    case Step::BURST:
        pBaseEnemy_->SetScale(Vector3::ZeroVector());
        pBaseEnemy_->DeathRenditionInit();
        step_ = Step::DEATH;
        break;

    /// -------------------------------------------------------
    /// 死
    ///---------------------------------------------------------
    case Step::DEATH:
        pBaseEnemy_->SetIsDeath(true);
        break;

    default:
        break;
    }
}

void EnemyDeath::Debug() {
}
