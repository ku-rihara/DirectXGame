/// behavior
#include "EnemyEscape.h"
#include "EnemyWait.h"
/// obj
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// math
#include "MathFunction.h"
/// imGui
#include <imgui.h>

EnemyChase::EnemyChase(BaseEnemy* enemy)
    : BaseEnemyBehavior("EnemyChase", enemy) {

    // 追跡アニメーションを再生
    pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);

    // 追跡タイマーを設定
    chaseTimer_ = pBaseEnemy_->GetParameter().chaseTime;
}

EnemyChase::~EnemyChase() {
}

void EnemyChase::Update() {
    const auto& param = pBaseEnemy_->GetParameter();

    // 追跡タイマーを減少
    chaseTimer_ -= KetaEngine::Frame::DeltaTimeRate();

    // 毎フレームプレイヤー方向を再計算して移動
    Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
    direction.y = 0.0f;
    direction.Normalize();
    pBaseEnemy_->AddPosition(direction * (param.chaseSpeed * KetaEngine::Frame::DeltaTime()));

    // 毎フレームプレイヤーを向く
    pBaseEnemy_->DirectionToPlayer();

    // 追跡時間が終了したらWaitに戻る
    if (chaseTimer_ <= 0.0f) {
        pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
    }
}

void EnemyChase::Debug() {
}
