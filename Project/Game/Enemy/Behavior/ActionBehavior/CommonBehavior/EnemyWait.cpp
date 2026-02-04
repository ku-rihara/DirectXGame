/// behavior
#include "EnemyWait.h"
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

    // 待機アニメーションにリセット
    pBaseEnemy_->ResetToWaitAnimation();

    //
    isMoveCollTime_ = true;
    moveCollTimer_  = pBaseEnemy_->GetParameter().attackCooldownTime;


}

EnemyWait::~EnemyWait() {
}

void EnemyWait::Update() {

    // 攻撃後待機時間の更新
    UpdateAttackCooldown();

    // 距離
    float distance = pBaseEnemy_->CalcDistanceToPlayer();
    distance;
    // プレイヤーの方向を向く
    pBaseEnemy_->DirectionToPlayer();

    // アクションを起こすまでクールタイムを設ける
    if (isMoveCollTime_) {
        return;
    }


    // プレイヤーが離れた時の行動切り替え
    pBaseEnemy_->OnPlayerDistantAction();

     // プレイヤーが近づいた時の行動切り替え
    pBaseEnemy_->OnPlayerApproachAction();
}

void EnemyWait::UpdateAttackCooldown() {
    if (!isMoveCollTime_) {
        return;
    }

    moveCollTimer_ -= KetaEngine::Frame::DeltaTimeRate();

    if (moveCollTimer_ <= 0.0f) {
        isMoveCollTime_ = false;
        moveCollTimer_  = 0.0f;
    }
}

void EnemyWait::Debug() {
}
