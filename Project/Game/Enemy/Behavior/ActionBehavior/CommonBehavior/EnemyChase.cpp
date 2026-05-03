/// behavior
#include "EnemyChase.h"
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

}

EnemyChase::~EnemyChase() {
}

void EnemyChase::Update() {
    
    // 毎フレームプレイヤー方向を再計算して移動
    Vector3 direction = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());
    direction.y = 0.0f;
    direction.Normalize();
    pBaseEnemy_->AddPosition(direction * (pBaseEnemy_->GetParameter().chaseSpeed * KetaEngine::Frame::DeltaTime()));

    // 毎フレームプレイヤーを向く
    pBaseEnemy_->DirectionToPlayer();

}

void EnemyChase::Debug() {
}
