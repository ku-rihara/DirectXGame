/// behavior
#include "EnemyDeath.h"
/// obj
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// math
#include "Frame/Frame.h"
#include "MathFunction.h"

///=========================================================
/// 初期化
///=========================================================
EnemyDeath::EnemyDeath(BaseEnemy* boss)
    : BaseEnemyDamageReaction("EnemyDeath", boss) {

    step_ = Step::DIRECTIONSET;
    SetUpSteps();
}

EnemyDeath::~EnemyDeath() {
}

void EnemyDeath::AngleCaluclation() {
    Vector3 playerPos = pBaseEnemy_->GetBaseInfo()->GetPlayer()->GetWorldPosition();
    direction_        = pBaseEnemy_->GetDirectionToTarget(playerPos);
    direction_.y      = 0.0f;
    direction_        = direction_.Normalize();
    objectiveAngle_   = CalcFaceAngleY(pBaseEnemy_->GetWorldPosition(), playerPos);
}

///=========================================================
/// 更新
///=========================================================
void EnemyDeath::Update([[maybe_unused]] float deltaTime) {
    auto it = stepPhase_.find(step_);
    if (it != stepPhase_.end()) {
        it->second();
    }
}

///=========================================================
/// ステップごとの処理
///=========================================================
void EnemyDeath::StepDirectionSet() {
    // 方向計算 → アニメーションへ
    AngleCaluclation();
    pBaseEnemy_->SetRotationY(objectiveAngle_);
    step_ = Step::ANIMATION;
}

void EnemyDeath::StepAnimation() {
    // 死亡アニメーション再生・終了待ち
    if (!deathAnimStarted_) {
        deathAnimStarted_            = true;
        deathAnimTimer_              = 0.0f;
        const std::string& deathAnim = pBaseEnemy_->GetAnimator()->GetAnimationName(BaseEnemy::AnimationType::Death);
        if (!deathAnim.empty()) {
            pBaseEnemy_->GetAnimator()->PlayAnimation(BaseEnemy::AnimationType::Death, false);
            pBaseEnemy_->GetAnimator()->GetAnimationObject()->SetAnimationEndCallback(deathAnim, [this]() {
                deathAnimFinished_ = true;
            });
        } else {
            // アニメーション未設定の場合はすぐに次へ
            deathAnimFinished_ = true;
        }
    }

    // 終了コールバックが来なくても、タイムアウトしたら強制的に次へ進める
    if (!deathAnimFinished_) {
        deathAnimTimer_ += KetaEngine::Frame::DeltaTime();
        if (deathAnimTimer_ >= pBaseEnemy_->GetBaseInfo()->GetParameter().deathAnimTimeout) {
            deathAnimFinished_ = true;
        }
    }

    if (deathAnimFinished_) {
        step_ = Step::BURST;
    }
}

void EnemyDeath::StepBurst() {
    // スケールを0にして爆散エフェクト開始
    pBaseEnemy_->SetScale(Vector3::ZeroVector());
    pBaseEnemy_->DeathRenditionInit();
    deathWaitTimer_ = 2.0f;
    step_           = Step::WAITING;
}

void EnemyDeath::StepWaiting() {
    // エフェクト終了を待つ
    deathWaitTimer_ -= KetaEngine::Frame::DeltaTime();
    if (deathWaitTimer_ <= 0.0f) {
        step_ = Step::DEATH;
    }
}

void EnemyDeath::StepDeath() {
    pBaseEnemy_->SetIsDeath(true);
}

///=========================================================
/// セットアップ
///=========================================================
void EnemyDeath::SetUpSteps() {
    stepPhase_[Step::DIRECTIONSET] = [this] { StepDirectionSet(); };
    stepPhase_[Step::ANIMATION]    = [this] { StepAnimation(); };
    stepPhase_[Step::BURST]        = [this] { StepBurst(); };
    stepPhase_[Step::WAITING]      = [this] { StepWaiting(); };
    stepPhase_[Step::DEATH]        = [this] { StepDeath(); };
}

///=========================================================
/// デバッグ
///=========================================================
void EnemyDeath::Debug() {
}
