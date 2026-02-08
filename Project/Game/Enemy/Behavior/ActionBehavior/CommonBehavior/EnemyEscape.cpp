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
/// std
#include <cmath>

EnemyEscape::EnemyEscape(BaseEnemy* enemy, const Vector3& escapeDirection)
    : BaseEnemyBehavior("EnemyEscape", enemy)
    , escapeDirection_(escapeDirection) {

    // 逃走アニメーションを再生
    pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Dash, true);

    // 逃走タイマーを設定
    escapeTimer_ = pBaseEnemy_->GetParameter().escapeTime;

    // 逃げる方向の目標角度を計算
    targetAngle_ = std::atan2(-escapeDirection_.x, -escapeDirection_.z);
}

EnemyEscape::~EnemyEscape() {
}

void EnemyEscape::Update() {
    const auto& param = pBaseEnemy_->GetParameter();

    // 逃走タイマーを減少
    escapeTimer_ -= KetaEngine::Frame::DeltaTimeRate();

    // 逃げる方向に移動
    pBaseEnemy_->AddPosition(escapeDirection_ * (param.escapeSpeed * KetaEngine::Frame::DeltaTime()));

    // 逃げる方向を向く
    float currentAngle = pBaseEnemy_->GetBaseRotationY();
    pBaseEnemy_->SetRotationY(LerpShortAngle(currentAngle, targetAngle_, 0.5f));

    // 逃走時間が終了したらWaitに戻る
    if (escapeTimer_ <= 0.0f) {
        pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
    }
}

void EnemyEscape::Debug() {
}
