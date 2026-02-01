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
EnemyWait::EnemyWait(BaseEnemy* boss, bool skipDiscoveryDelay)
    : BaseEnemyBehavior("EnemyWait", boss) {

    // 待機アニメーションにリセット
    pBaseEnemy_->ResetToWaitAnimation();

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

    // プレイヤー発見状態の初期化
    // skipDiscoveryDelayがtrueの場合（攻撃後やChaseから戻った場合）は
    // 発見済みとして遅延をスキップ
    if (skipDiscoveryDelay) {
        hasDiscoveredPlayer_ = true;
        discoveryDelayTimer_ = pBaseEnemy_->GetParameter().discoveryDelayTime; // 遅延完了状態に
    } else {
        hasDiscoveredPlayer_ = false;
        discoveryDelayTimer_ = 0.0f;
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

    // プレイヤーが追跡範囲内に入った場合
    if (distance_ < pBaseEnemy_->GetParameter().chaseDistance) {
        // プレイヤーを初めて発見した場合
        if (!hasDiscoveredPlayer_) {
            hasDiscoveredPlayer_ = true;
            discoveryDelayTimer_ = 0.0f;

            // 発見時のアニメーション（予備動作）を開始
            pBaseEnemy_->InitChaseAnimation();

            // 発見エフェクトを再生
            if (pBaseEnemy_->GetEnemyEffects()) {
                pBaseEnemy_->GetEnemyEffects()->Emit("DiscoveryEffect");
            }
        }

        // 発見後の遅延時間をカウント
        discoveryDelayTimer_ += KetaEngine::Frame::DeltaTimeRate();

        // 遅延時間が経過したら追跡開始
        if (discoveryDelayTimer_ >= pBaseEnemy_->GetParameter().discoveryDelayTime) {
            pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyChasePlayer>(pBaseEnemy_));
            return;
        }

        // 遅延中はプレイヤーの方を向くのみ
        direction.y = 0.0f;
        direction.Normalize();
        float objectiveAngle = std::atan2(-direction.x, -direction.z);
        pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseTransform().rotation_.y, objectiveAngle, 0.3f));
        return;
    } else {
        // プレイヤーが範囲外に出たらリセット
        hasDiscoveredPlayer_ = false;
        discoveryDelayTimer_ = 0.0f;
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
        ImGui::Text("Chase Distance: %.2f", pBaseEnemy_->GetParameter().chaseDistance);
        ImGui::Text("Is in Attack Cooldown: %s", isInAttackCooldown_ ? "true" : "false");
        ImGui::Text("Attack Cooldown Timer: %.2f", attackCooldownTimer_);
        ImGui::Separator();
        ImGui::Text("Has Discovered Player: %s", hasDiscoveredPlayer_ ? "true" : "false");
        ImGui::Text("Discovery Delay Timer: %.2f / %.2f", discoveryDelayTimer_, pBaseEnemy_->GetParameter().discoveryDelayTime);

        // 状態表示
        if (distance_ < pBaseEnemy_->GetParameter().chaseDistance) {
            if (discoveryDelayTimer_ < pBaseEnemy_->GetParameter().discoveryDelayTime) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "State: Waiting (Discovery Delay)");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "State: Ready to Chase");
            }
        } else {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "State: Player out of range");
        }
        ImGui::TreePop();
    }
#endif
}
