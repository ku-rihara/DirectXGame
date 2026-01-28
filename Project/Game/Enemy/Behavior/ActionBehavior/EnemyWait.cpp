/// behavior
#include "EnemyWait.h"
#include "EnemyChasePlayer.h"

/// math
#include "MathFunction.h"
/// obj
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>
/// std
#include <cmath>

// 初期化
EnemyWait::EnemyWait(BaseEnemy* boss)
    : BaseEnemyBehavior("EnemyWait", boss) {

    spriteEase_.Init("EnemyOverLookSpriteScaling.json");
    spriteEase_.SetAdaptValue(&tempSpriteScale_);
    spriteEase_.Reset();

    isChase_ = true;

    // 攻撃後待機時間の初期化
    // 攻撃から戻ってきた場合は待機時間を設定
    if (pBaseEnemy_->GetIsReturningFromAttack()) {
        isInAttackCooldown_ = true;
        attackCooldownTimer_ = pBaseEnemy_->GetParameter().attackCooldownTime;
        // フラグをリセット
        pBaseEnemy_->SetIsReturningFromAttack(false);
    } else {
        isInAttackCooldown_ = false;
        attackCooldownTimer_ = 0.0f;
    }
}

EnemyWait::~EnemyWait() {
}

void EnemyWait::Update() {

    spriteEase_.Update(KetaEngine::Frame::DeltaTimeRate());

    // 攻撃後待機時間の更新
    UpdateAttackCooldown();

    // 攻撃クールダウン中は追従しない
    if (isInAttackCooldown_) {
        // プレイヤーの方を向く処理のみ
        Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
        direction.y = 0.0f;
        direction.Normalize();

        float objectiveAngle = std::atan2(-direction.x, -direction.z);
        pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseTransform().rotation_.y, objectiveAngle, 0.3f));
        return;
    }

    // ターゲットへのベクトル
    Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

    // 距離
    distance_ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

    // 近すぎる場合は追従を開始
    if (distance_ < pBaseEnemy_->GetParameter().chaseDistance) {
        pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyChasePlayer>(pBaseEnemy_));
        return;
    }

    // 正規化
    direction.y = 0.0f;
    direction.Normalize();

    // 目標角度を計算
    float objectiveAngle = std::atan2(-direction.x, -direction.z);

    // 最短角度補間でプレイヤーの回転を更新
    pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseTransform().rotation_.y, objectiveAngle, 0.3f));
}

void EnemyWait::UpdateAttackCooldown() {
    if (!isInAttackCooldown_) {
        return;
    }

    attackCooldownTimer_ -= KetaEngine::Frame::DeltaTimeRate();

    if (attackCooldownTimer_ <= 0.0f) {
        isInAttackCooldown_ = false;
        attackCooldownTimer_ = 0.0f;
    }
}

void EnemyWait::Debug() {
#ifdef _DEBUG
    if (ImGui::TreeNode("EnemyWait")) {
        ImGui::Text("Distance: %.2f", distance_);
        ImGui::Text("Is in Attack Cooldown: %s", isInAttackCooldown_ ? "true" : "false");
        ImGui::Text("Attack Cooldown Timer: %.2f", attackCooldownTimer_);
        ImGui::TreePop();
    }
#endif
}
