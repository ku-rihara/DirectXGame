/// behavior
#include "NormalEnemyRetreat.h"
#include "../CommonBehavior/EnemyAttack.h"

/// math
#include "MathFunction.h"
/// obj
#include "Game/Enemy/Types/NormalEnemy.h"
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>
/// std
#include <cmath>

NormalEnemyRetreat::NormalEnemyRetreat(NormalEnemy* enemy)
    : NormalEnemyBehaviorBase("NormalEnemyRetreat", enemy) {

    distance_ = 0.0f;
    isRetreatStarted_ = false;

    // 攻撃前待機初期化
    isWaitingForPreAttack_ = false;
    preAttackWaitTimer_ = 0.0f;

    // PreDash状態初期化
    isPreDashStarted_ = false;

    // スケールイージング初期化
    scaleEasing_.Init("EnemyFindScaling.json");
    scaleEasing_.SetAdaptValue(&tempEnemyScale_);
    scaleEasing_.SetStartValue(pBaseEnemy_->GetParameter().baseScale_);
    scaleEasing_.Reset();

    // ダッシュアニメーション開始（後退中も走るモーション）
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
    distance_ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

    // プレイヤーの方を向く処理（共通）
    auto facePlayer = [&]() {
        direction.y = 0.0f;
        direction.Normalize();
        float objectiveAngle = std::atan2(-direction.x, -direction.z);
        pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseTransform().rotation_.y, objectiveAngle, 0.3f));
    };

    // PreDash中の処理
    if (isPreDashStarted_) {
        // PreDashアニメーション終了チェック
        if (pBaseEnemy_->IsPreDashFinished()) {
            // 攻撃開始
            pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyAttack>(pBaseEnemy_));
            return;
        }

        facePlayer();
        return;
    }

    // 攻撃前待機中の処理（攻撃範囲到達後、PreDash開始までの待機時間）
    if (isWaitingForPreAttack_) {
        preAttackWaitTimer_ -= KetaEngine::Frame::DeltaTimeRate();

        facePlayer();

        if (preAttackWaitTimer_ <= 0.0f) {
            isWaitingForPreAttack_ = false;
            preAttackWaitTimer_ = 0.0f;

            // PreDash開始
            isPreDashStarted_ = true;

            // PreDashアニメーション開始
            pBaseEnemy_->InitChaseAnimation();

            // 発見エフェクトを再生
            if (pBaseEnemy_->GetEnemyEffects()) {
                pBaseEnemy_->GetEnemyEffects()->Emit("DiscoveryEffect");
            }
        }
        return;
    }

    // 攻撃可能範囲に到達したら攻撃前待機開始
    if (IsInAttackRange(distance_)) {
        isWaitingForPreAttack_ = true;
        preAttackWaitTimer_ = param.preAttackWaitTime;
        facePlayer();
        return;
    }

    //// 攻撃範囲より遠すぎる場合はWaitに戻る（プレイヤーが逃げた場合）
    //if (distance_ > param.attackRangeMax * 1.5f) {
    //    pBaseEnemy_->ChangeBehavior(std::make_unique<NormalEnemyWait>(pBaseEnemy_, true));
    //    return;
    //}

    // 後退処理（プレイヤーと反対方向に移動）
    direction.y = 0.0f;
    direction.Normalize();

    // プレイヤーから離れる方向に移動
    Vector3 retreatDirection = -direction;
    pBaseEnemy_->AddPosition(retreatDirection * (param.retreatSpeed * KetaEngine::Frame::DeltaTime()));

    // 進行方向（後退方向）を向く
    float retreatAngle = std::atan2(-retreatDirection.x, -retreatDirection.z);
    pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseTransform().rotation_.y, retreatAngle, 0.3f));
}

bool NormalEnemyRetreat::IsInAttackRange(float distance) const {
    const auto& param = pBaseEnemy_->GetParameter();
    return distance >= param.attackRangeMin && distance <= param.attackRangeMax;
}

void NormalEnemyRetreat::Debug() {
#ifdef _DEBUG
    if (ImGui::TreeNode("NormalEnemyRetreat")) {
        const auto& param = pBaseEnemy_->GetParameter();

        ImGui::Text("Distance: %.2f", distance_);
        ImGui::Separator();
        ImGui::Text("Attack Range: %.2f - %.2f", param.attackRangeMin, param.attackRangeMax);
        ImGui::Text("Retreat Speed: %.2f", param.retreatSpeed);
        ImGui::Separator();
        ImGui::Text("Pre-Attack Wait: %s (%.2f / %.2f)",
            isWaitingForPreAttack_ ? "Active" : "Done",
            preAttackWaitTimer_, param.preAttackWaitTime);
        ImGui::Text("PreDash Started: %s", isPreDashStarted_ ? "true" : "false");

        // 状態表示
        ImGui::Separator();
        if (isWaitingForPreAttack_) {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "State: Pre-Attack Wait");
        } else if (isPreDashStarted_) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "State: PreDash Animation");
        } else if (IsInAttackRange(distance_)) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "State: In Attack Range");
        } else if (distance_ < param.attackRangeMin) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "State: Retreating (Too Close)");
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "State: Retreating");
        }

        ImGui::TreePop();
    }
#endif
}
