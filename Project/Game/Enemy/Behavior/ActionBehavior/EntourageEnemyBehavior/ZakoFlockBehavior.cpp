#include "ZakoFlockBehavior.h"

#include "CollisionBox/GameColliderType.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/EntourageEnemy.h"
#include "Enemy/Types/LeaderEnemy.h"
#include "Frame/Frame.h"
#include "MathFunction.h"

#include <cmath>

namespace {
// ボスがダメージリアクション中かどうか
bool IsBossInDamageMotion(BaseEnemy* boss) {
    auto* reaction = boss->GetDamageReactionBehavior();
    return reaction && !reaction->IsReactionRoot();
}

bool IsBossFleeing(BaseEnemy* boss) {
    if (!HasColliderType(boss, GameColliderType::LeaderEnemy)) {
        return false;
    }
    return static_cast<LeaderEnemy*>(boss)->IsFleeing();
}
} 

ZakoFlockBehavior::ZakoFlockBehavior(EntourageEnemy* enemy)
    : BaseEnemyBehavior("ZakoFlock", static_cast<BaseEnemy*>(enemy)), pEntourageEnemy_(enemy) {

    // 生成時点で定位置との距離を見てアニメーションを決定する
    BaseEnemy* boss = pEntourageEnemy_->GetBoss();
    if (boss && !boss->GetIsDeath() && !IsBossInDamageMotion(boss)) {
        Vector3 diff = pEntourageEnemy_->CalcFlockSlotPosition() - pBaseEnemy_->GetWorldPosition();
        diff.y       = 0.0f;
        if (diff.Length() > kCloseEnough || IsBossFleeing(boss)) {
            pBaseEnemy_->GetAnimator()->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
            isRunning_ = true;
        } else {
            pBaseEnemy_->GetAnimator()->ResetToWaitAnimation();
        }
    } else {
        pBaseEnemy_->GetAnimator()->ResetToWaitAnimation();
    }
}

void ZakoFlockBehavior::Update() {
    float dt = KetaEngine::Frame::DeltaTime();

    BaseEnemy* boss = pEntourageEnemy_->GetBoss();
    if (!boss || boss->GetIsDeath()) {
        return;
    }

    // ボスがダメージモーション中は動かず待機を維持する
    if (IsBossInDamageMotion(boss)) {
        if (isRunning_) {
            pBaseEnemy_->GetAnimator()->ResetToWaitAnimation();
            isRunning_ = false;
        }
        pBaseEnemy_->DirectionToPlayer();
        return;
    }

    // 定位置（Flockスロット）への目標位置を算出
    Vector3 targetPos = pEntourageEnemy_->CalcFlockSlotPosition();

    // 目標位置への距離を算出
    Vector3 diff = targetPos - pBaseEnemy_->GetWorldPosition();
    diff.y       = 0.0f;
    float dist   = diff.Length();

    //ボスが逃走中は、ボスの現在位置を追い続ける
    if (dist > kCloseEnough || IsBossFleeing(boss)) {
        // 目標方向へ滑らかに旋回
        float faceAngle = LerpShortAngle(
            pBaseEnemy_->GetBaseRotationY(),
            CalcFaceAngleY(pBaseEnemy_->GetWorldPosition(), targetPos, true),
            0.8f);
        pBaseEnemy_->SetRotationY(faceAngle);

        // 旋回済みの自身の向きへ前進する
        Vector3 moveDir(-std::sin(faceAngle), 0.0f, -std::cos(faceAngle));
        float speed = pBaseEnemy_->GetBaseInfo()->GetParameter().chaseSpeed;
        pBaseEnemy_->AddPosition(moveDir * (speed * dt));

        // アニメーションがDashでなければ再生
        auto animObj         = pBaseEnemy_->GetAnimator()->GetAnimationObject();
        std::string dashAnim = pBaseEnemy_->GetAnimator()->GetAnimationName(BaseEnemy::AnimationType::Dash);
        if (animObj && animObj->GetCurrentAnimationName() != dashAnim) {
            pBaseEnemy_->GetAnimator()->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
            isRunning_ = true;
        }
    } else {
        if (isRunning_) {
            pBaseEnemy_->GetAnimator()->ResetToWaitAnimation();
            isRunning_ = false;
        }
        // 停止中はプレイヤーを向く
        pBaseEnemy_->DirectionToPlayer();
    }
}

void ZakoFlockBehavior::Debug() {
}