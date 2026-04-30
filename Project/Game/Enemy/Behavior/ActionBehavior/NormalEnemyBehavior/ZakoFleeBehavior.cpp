#include "ZakoFleeBehavior.h"

#include "Enemy/Types/NormalEnemy.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
#include "Frame/Frame.h"
#include "MathFunction.h"

ZakoFleeBehavior::ZakoFleeBehavior(NormalEnemy* enemy)
    : BaseEnemyBehavior("ZakoFlee", static_cast<BaseEnemy*>(enemy))
    , pNormalEnemy_(enemy) {

    pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
}

void ZakoFleeBehavior::Update() {
    float dt = KetaEngine::Frame::DeltaTimeRate();

    // プレイヤーと逆方向に逃げる
    Vector3 toPlayer = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
    toPlayer.y       = 0.0f;
    toPlayer.Normalize();
    Vector3 fleeDir = toPlayer * -1.0f;

    float ramp  = (std::min)(fleeTimer_ / kSpeedRampUp, 1.0f);
    pBaseEnemy_->AddPosition(fleeDir * (pBaseEnemy_->GetParameter().fleeSpeed * ramp * dt));

    // 逃げる方向を向く
    float targetAngle  = std::atan2(-fleeDir.x, -fleeDir.z);
    float currentAngle = pBaseEnemy_->GetBaseRotationY();
    pBaseEnemy_->SetRotationY(LerpShortAngle(currentAngle, targetAngle, 0.8f));

    // 一定時間後に消える
    fleeTimer_ += dt;
    if (fleeTimer_ >= kMaxFleeTime) {
        pBaseEnemy_->SetIsDeath(true);
    }
}

void ZakoFleeBehavior::Debug() {
}
