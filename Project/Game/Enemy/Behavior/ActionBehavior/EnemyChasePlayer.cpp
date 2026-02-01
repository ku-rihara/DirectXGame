// behavior
#include "EnemyChasePlayer.h"
#include "EnemyAttack.h"
#include "EnemyWait.h"
// math
#include "MathFunction.h"
// Enemy
#include "Enemy/EnemyManager.h"
#include "Enemy/Types/BaseEnemy.h"
// Player
#include "Player/Player.h"
// frame
#include "Frame/Frame.h"
// std
#include <cmath>
// imGui
#include <imgui.h>

// 初期化
EnemyChasePlayer::EnemyChasePlayer(BaseEnemy* boss)
    : BaseEnemyBehavior("EnemyChasePlayer", boss) {

    isChase_ = true; // デバッグ用

    scaleEasing_.Init("EnemyFindScaling.json");
    scaleEasing_.SetAdaptValue(&tempEnemyScale_);
    scaleEasing_.SetStartValue(pBaseEnemy_->GetParameter().baseScale_);
    scaleEasing_.Reset();

    isChaseStarted_ = false; // 追跡開始フラグ初期化
}

EnemyChasePlayer::~EnemyChasePlayer() {
}

void EnemyChasePlayer::Update() {

    // 追跡開始時のアニメーション初期化
    if (!isChaseStarted_) {
        pBaseEnemy_->InitChaseAnimation();
        isChaseStarted_ = true;
    }

    // Enemy AmplitudeScaling
    scaleEasing_.Update(KetaEngine::Frame::DeltaTimeRate());
    pBaseEnemy_->SetScale(tempEnemyScale_);

    // 追跡中のアニメーション更新
    pBaseEnemy_->UpdateChaseAnimation(KetaEngine::Frame::DeltaTimeRate());

    // ターゲットへのベクトル
    Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

    // 距離計算
    distance_ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

    // 攻撃範囲内に入ったら攻撃開始
    if (distance_ <= pBaseEnemy_->GetParameter().attackStartDistance) {
        // 攻撃ビヘイビアに遷移
        if (pBaseEnemy_->GetChaseAnimState() == BaseEnemy::ChaseAnimationState::DASHING) {
            pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyAttack>(pBaseEnemy_));
            return;
        }
    }

    // PreDash中は移動せず、向きだけ変える
    if (pBaseEnemy_->GetChaseAnimState() == BaseEnemy::ChaseAnimationState::PRE_DASH) {
        // 正規化
        direction.y = 0.0f;
        direction.Normalize();

        // 目標角度を計算
        float objectiveAngle = std::atan2(-direction.x, -direction.z);

        // 最短角度補間でプレイヤーの回転を更新
        pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseTransform().rotation_.y, objectiveAngle, 0.3f));
        return;
    }

    // 正規化
    direction.y = 0.0f;
    direction.Normalize();

    /// 変位加算
    pBaseEnemy_->AddPosition(direction * (pBaseEnemy_->GetParameter().chaseSpeed * KetaEngine::Frame::DeltaTime()));

    // 目標角度を計算
    float objectiveAngle = std::atan2(-direction.x, -direction.z);

    // 最短角度補間でプレイヤーの回転を更新
    pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetBaseTransform().rotation_.y, objectiveAngle, 0.3f));
}

void EnemyChasePlayer::Debug() {
}
