/// behavior
#include "EnemyWait.h"
#include "EnemyChase.h"
/// obj
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
/// animation
#include "3D/AnimationObject3D/Object3DAnimation.h"
/// frame
#include "Frame/Frame.h"
/// imGui
#include <imgui.h>

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

    // プレイヤーの方向を向く
    pBaseEnemy_->DirectionToPlayer();

    // プレイヤーが追跡範囲内にいるか
    if (IsDiscovery()) {
        // 発見モーションを再生
        pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Discovery, false);

        currentPhase_ = [this]() {
            UpdateDiscovery();
        };
    }
}

bool EnemyWait::IsDiscovery() {
    const auto& param = pBaseEnemy_->GetParameter();
    bool result       = false;

    // 追跡開始距離以内＆追跡限界距離以上
    if (distance_ <= param.chaseDistance && distance_ >= param.chaseDistanceMin) {
        result = true;
    }
    return result;
}

void EnemyWait::UpdateDiscovery() {
}

void EnemyWait::UpdateEnd() {
    // EnemyChaseに遷移
    pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyChase>(pBaseEnemy_));
}

void EnemyWait::Debug() {
}
