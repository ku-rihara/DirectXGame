#include "StrongEnemyFleeBehavior.h"

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/StrongEnemy.h"
#include "Field/Field.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include "Player/Player.h"
#include <cmath>

StrongEnemyFleeBehavior::StrongEnemyFleeBehavior(StrongEnemy* enemy, float startDelay)
    : StrongEnemyBehaviorBase("StrongEnemyFleeBehavior", enemy)
    , startDelayTimer_(startDelay) {

    if (startDelayTimer_ <= 0.0f) {
        pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
    }
    // delay中は現在のアニメーションを引き継ぐ（Waitに切り替えない）
}

void StrongEnemyFleeBehavior::Update() {
    const float dt    = KetaEngine::Frame::DeltaTime();
    const auto& param = GetEnemy()->GetStrongParameter();

    // ── クールタイム中 ──────────────────────────────────
    if (startDelayTimer_ > 0.0f) {
        startDelayTimer_ -= dt;
        pBaseEnemy_->DirectionToPlayer();
        if (startDelayTimer_ <= 0.0f) {
            pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
        }
        return;
    }

    // ── Taunt遷移: プレイヤーとの距離が fleeDistance 以上 ────
    if (pBaseEnemy_->CalcDistanceToPlayer() >= param.fleeDistance) {
        GetEnemy()->StartTaunt();
        return;
    }

    // ── プレイヤーから遠ざかる方向を計算 ─────────────────────
    Vector3 toPlayer = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
    toPlayer.y       = 0.0f;
    const float toPlayerLen = toPlayer.Length();
    if (toPlayerLen < 0.001f) {
        return; // プレイヤーと重なっている場合は何もしない
    }
    toPlayer = toPlayer * (1.0f / toPlayerLen);
    const Vector3 fleeDir = toPlayer * -1.0f;

    // ── 境界距離を計算してブレンド補正 ───────────────────────
    Vector3 pos = pBaseEnemy_->GetWorldPosition();
    pos.y       = 0.0f;

    const Vector3& fieldScale    = Field::baseScale_;
    const float radiusX          = fieldScale.x - pBaseEnemy_->GetParameter().baseScale_.x;
    const float radiusZ          = fieldScale.z - pBaseEnemy_->GetParameter().baseScale_.z;
    const float dX               = radiusX - std::abs(pos.x);
    const float dZ               = radiusZ - std::abs(pos.z);
    const float distFromBoundary = dX < dZ ? dX : dZ;

    Vector3 moveDir = fleeDir;

    if (distFromBoundary < param.fleeDistance) {
        // 境界が近い → 中心方向へのブレンド比率を上げる
        // distFromBoundary=0(壁) のとき t=1(純中心方向)、fleeDistance のとき t=0(純逃走方向)
        const float clampedDist = distFromBoundary > 0.0f ? distFromBoundary : 0.0f;
        const float t           = 1.0f - clampedDist / param.fleeDistance;

        Vector3 toCenter = -pos;
        toCenter.y       = 0.0f;
        const float len  = toCenter.Length();
        if (len > 0.001f) {
            toCenter = toCenter * (1.0f / len);
            moveDir  = fleeDir * (1.0f - t) + toCenter * t;
            const float moveDirLen = moveDir.Length();
            if (moveDirLen > 0.001f) {
                moveDir = moveDir * (1.0f / moveDirLen);
            } else {
                moveDir = toCenter;
            }
        }
    }

    // ── 進行方向を向いて移動 ─────────────────────────────────
    const float targetAngle = std::atan2(moveDir.x, moveDir.z);
    pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseRotationY(), targetAngle, 0.8f));
    pBaseEnemy_->AddPosition(moveDir * (param.fleeSpeed * dt));
}

void StrongEnemyFleeBehavior::Debug() {}
