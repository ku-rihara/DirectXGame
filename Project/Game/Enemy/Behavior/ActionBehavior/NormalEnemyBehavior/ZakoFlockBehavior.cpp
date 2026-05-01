#include "ZakoFlockBehavior.h"

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/NormalEnemy.h"
#include "Frame/Frame.h"
#include "MathFunction.h"

#include <cmath>

ZakoFlockBehavior::ZakoFlockBehavior(NormalEnemy* enemy)
    : BaseEnemyBehavior("ZakoFlock", static_cast<BaseEnemy*>(enemy)), pNormalEnemy_(enemy) {

    pBaseEnemy_->ResetToWaitAnimation();
}

void ZakoFlockBehavior::Update() {
    float dt = KetaEngine::Frame::DeltaTime();

    BaseEnemy* boss = pNormalEnemy_->GetBoss();
    if (!boss || boss->GetIsDeath()) {
        return;
    }

    // スポーン時のローカルオフセットをボスの現在Y回転で変換してワールド目標位置を算出
    Vector3 localOffset = pNormalEnemy_->GetSpawnOffset();
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

    Vector3 diff = targetPos - pBaseEnemy_->GetWorldPosition();
    diff.y       = 0.0f;
    float dist   = diff.Length();

    if (dist > kCloseEnough) {
        Vector3 dir = diff;
        dir.Normalize();
        float speed = pBaseEnemy_->GetParameter().chaseSpeed;
        pBaseEnemy_->AddPosition(dir * (speed * dt));

        // 移動中は進行方向を向く
        float targetAngle  = std::atan2(-dir.x, -dir.z);
        float currentAngle = pBaseEnemy_->GetBaseRotationY();
        pBaseEnemy_->SetRotationY(LerpShortAngle(currentAngle, targetAngle, 0.8f));

        if (!isRunning_) {
            pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
            isRunning_ = true;
        }
    } else {
        if (isRunning_) {
            pBaseEnemy_->ResetToWaitAnimation();
            isRunning_ = false;
        }
        // 停止中はプレイヤーを向く
        pBaseEnemy_->DirectionToPlayer();
    }
}

void ZakoFlockBehavior::Debug() {
}
