#include "ZakoFlockBehavior.h"

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/EntourageEnemy.h"
#include "Field/Field.h"
#include "Frame/Frame.h"
#include "MathFunction.h"

#include <algorithm>

ZakoFlockBehavior::ZakoFlockBehavior(EntourageEnemy* enemy)
    : BaseEnemyBehavior("ZakoFlock", static_cast<BaseEnemy*>(enemy)), pEntourageEnemy_(enemy) {

    // 生成時点でボスとの距離を見てアニメーションを決定する
    BaseEnemy* boss = pEntourageEnemy_->GetBoss();
    if (boss && !boss->GetIsDeath()) {
        Vector3 diff = boss->GetWorldPosition() - pBaseEnemy_->GetWorldPosition();
        diff.y       = 0.0f;
        if (diff.Length() > kCloseEnough) {
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

    // スポーン時のローカルオフセットをボスの現在Y回転で変換してワールド目標位置を算出
    Vector3 localOffset = pEntourageEnemy_->GetSpawnOffset();
    localOffset.y       = 0.0f;

    float bossRotY = boss->GetBaseRotationY();
    float cosY     = cosf(bossRotY);
    float sinY     = sinf(bossRotY);
    Vector3 worldOffset(
        cosY * localOffset.x + sinY * localOffset.z,
        0.0f,
        -sinY * localOffset.x + cosY * localOffset.z);

    Vector3 targetPos = boss->GetWorldPosition() + worldOffset;
    targetPos.y       = pBaseEnemy_->GetWorldPosition().y;

    // 目標位置をフィールド境界内に制限

    const float rx = Field::baseScale_.x - pBaseEnemy_->GetBaseInfo()->GetParameter().baseScale_.x;
    const float rz = Field::baseScale_.z - pBaseEnemy_->GetBaseInfo()->GetParameter().baseScale_.z;
    if (rx > 0.0f) {
        targetPos.x = std::clamp(targetPos.x, -rx, rx);
    }
    if (rz > 0.0f) {
        targetPos.z = std::clamp(targetPos.z, -rz, rz);
    }

    // 目標位置への距離と方向を算出
    Vector3 diff = targetPos - pBaseEnemy_->GetWorldPosition();
    diff.y       = 0.0f;
    float dist   = diff.Length();

    // 十分に近づいていなければ移動
    if (dist > kCloseEnough) {
        Vector3 dir = diff;
        dir.Normalize();
        float speed = pBaseEnemy_->GetBaseInfo()->GetParameter().chaseSpeed;
        pBaseEnemy_->AddPosition(dir * (speed * dt));

        // 移動中は進行方向を向く
        pBaseEnemy_->SetRotationY(LerpShortAngle(
            pBaseEnemy_->GetBaseRotationY(),
            CalcFaceAngleY(pBaseEnemy_->GetWorldPosition(), targetPos, true),
            0.8f));

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