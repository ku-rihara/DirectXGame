#include "ZakoFlockBehavior.h"

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/NormalEnemy.h"
#include "Frame/Frame.h"
#include "MathFunction.h"

#include <cmath>
#include <numbers>

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

    // スロット角度からフォーメーション目標位置を計算
    int32_t slotIdx   = pNormalEnemy_->GetSlotIndex();
    int32_t slotTotal = (std::max)(pNormalEnemy_->GetSlotCount(), 1);
    float angle       = static_cast<float>(slotIdx) * (2.0f * std::numbers::pi_v<float> / static_cast<float>(slotTotal));
    Vector3 slotOffset(cosf(angle) * kFormationRadius, 0.0f, sinf(angle) * kFormationRadius);

    Vector3 targetPos = boss->GetWorldPosition() + slotOffset;
    targetPos.y       = pBaseEnemy_->GetWorldPosition().y;

    Vector3 diff = targetPos - pBaseEnemy_->GetWorldPosition();
    diff.y       = 0.0f;
    float dist   = diff.Length();

    if (dist > kCloseEnough) {
        Vector3 dir = diff;
        dir.Normalize();
        float speed = (std::min)(dist * kSpringFactor, kMaxFlockSpeed);
        pBaseEnemy_->AddPosition(dir * (speed * dt));

        if (!isRunning_) {
            pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
            isRunning_ = true;
        }
    } else {
        if (isRunning_) {
            pBaseEnemy_->ResetToWaitAnimation();
            isRunning_ = false;
        }
    }

    // 常にプレイヤーを向く
    pBaseEnemy_->DirectionToPlayer();
}

void ZakoFlockBehavior::Debug() {
}
