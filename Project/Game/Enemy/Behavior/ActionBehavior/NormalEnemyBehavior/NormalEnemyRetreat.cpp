/// behavior
#include "NormalEnemyRetreat.h"
#include "../CommonBehavior/EnemyAttack.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"

/// math
#include "MathFunction.h"
/// obj
#include "Enemy/Types/NormalEnemy.h"
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>
/// std
#include <cmath>

NormalEnemyRetreat::NormalEnemyRetreat(NormalEnemy* enemy)
    : NormalEnemyBehaviorBase("NormalEnemyRetreat", enemy) {

    distance_         = 0.0f;
    isRetreatStarted_ = false;

    // 攻撃前待機初期化
    isWaitingForPreAttack_ = false;
    preAttackWaitTimer_    = 0.0f;

    // スケールイージング初期化
    scaleEasing_.Init("EnemyFindScaling.json");
    scaleEasing_.SetAdaptValue(&tempEnemyScale_);
    scaleEasing_.SetStartValue(pBaseEnemy_->GetParameter().baseScale_);
    scaleEasing_.Reset();

    // ダッシュアニメーション開始
    pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);
}

NormalEnemyRetreat::~NormalEnemyRetreat() {
}

void NormalEnemyRetreat::Update() {
    const auto& param = pBaseEnemy_->GetParameter();

    // スケールイージング更新
    scaleEasing_.Update(KetaEngine::Frame::DeltaTimeRate());
    pBaseEnemy_->SetScale(tempEnemyScale_);

    // プレイヤーへの方向と距離を計算
    Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
    distance_         = pBaseEnemy_->CalcDistanceToPlayer();
    pBaseEnemy_->DirectionToPlayer(true);

    // 攻撃可能範囲に到達したら攻撃前待機開始
    if (IsInAttackRange(distance_)) {
        isWaitingForPreAttack_ = true;
        preAttackWaitTimer_    = param.preAttackWaitTime;
        return;
    }

    // プレイヤーと反対方向に移動
    direction.y = 0.0f;
    direction.Normalize();

    // プレイヤーから離れる方向に移動
    Vector3 retreatDirection = -direction;
    pBaseEnemy_->AddPosition(retreatDirection * (param.retreatSpeed * KetaEngine::Frame::DeltaTime()));
}

bool NormalEnemyRetreat::IsInAttackRange(float distance) const {
    const auto& param = pBaseEnemy_->GetParameter();
    return distance >= param.attackRangeMin && distance <= param.attackRangeMax;
}

void NormalEnemyRetreat::Debug() {
}
