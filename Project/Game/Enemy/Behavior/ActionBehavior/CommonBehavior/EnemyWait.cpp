/// behavior
#include "EnemyWait.h"
#include "EnemyEscape.h"
/// obj
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// animation
#include "3D/AnimationObject3D/Object3DAnimation.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>
/// std
#include <cmath>

// 初期化
EnemyWait::EnemyWait(BaseEnemy* boss)
    : BaseEnemyBehavior("EnemyWait", boss) {

    // 待機アニメーションにリセット
    pBaseEnemy_->ResetToWaitAnimation();

    // 初期フェーズを待機に設定
    currentPhase_ = [this]() {
        UpdateWaiting();
    };

    // アニメーション終了時のコールバックを設定
    const std::string& animeName = pBaseEnemy_->GetAnimationName(BaseEnemy::AnimationType::Discovery);
    pBaseEnemy_->GetAnimationObject()->SetAnimationEndCallback(animeName, [this]() {
        currentPhase_ = [this]() {
            UpdateEnd();
        };
    });

    escapeDirection_ = Vector3::ZeroVector();
}

EnemyWait::~EnemyWait() {
    // コールバックをクリア
    pBaseEnemy_->GetAnimationObject()->ClearAllAnimationEndCallbacks();
}

void EnemyWait::Update() {
    // 距離を計算
    distance_ = pBaseEnemy_->CalcDistanceToPlayer();

    // 現在のフェーズを実行
    if (currentPhase_) {
        currentPhase_();
    }
}

void EnemyWait::UpdateWaiting() {
    const auto& param = pBaseEnemy_->GetParameter();

    // プレイヤーの方向を向く
    pBaseEnemy_->DirectionToPlayer();

    // プレイヤーが発見範囲内にいるか
    if (distance_ <= param.escapeDistance) {
        // 逃げる方向を計算（プレイヤーと反対方向）
        Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
        direction.y       = 0.0f;
        direction.Normalize();
        escapeDirection_ = -direction; // 反対方向

        // 発見モーションを再生
        pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Discovery, false);

        //
        currentPhase_ = [this]() {
            UpdateDiscovery();
        };
    }
}

void EnemyWait::UpdateDiscovery() {
   
}

void EnemyWait::UpdateEnd() {
    // EnemyEscapeに遷移
    pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyEscape>(pBaseEnemy_, escapeDirection_));
}

void EnemyWait::Debug() {
}
