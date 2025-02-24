#include "BaseEnemyBehavior.h"
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"



void BaseEnemyBehavior::AngleCaluclation() {
	// ターゲットへのベクトル
	direction_ = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

	direction_.y = 0.0f;
	direction_.Normalize();

	// 目標角度を計算
	objectiveAngle_ = std::atan2(direction_.x, -direction_.z);

}