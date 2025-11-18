
/// behavior
#include"EnemyWait.h"
#include"EnemyChasePlayer.h"

/// math
#include"MathFunction.h"
/// obj
#include"Enemy/Types/BaseEnemy.h"
#include"Player/Player.h"
/// frame
#include"Frame/Frame.h"
/// imgui
#include<imgui.h>
/// std
#include<cmath> 

//初期化
EnemyWait::EnemyWait(BaseEnemy* boss)
	: BaseEnemyBehavior("EnemyWait", boss) {
	

	//パラメータ初期化
	pBaseEnemy_->GetNotFindSprite()->SetScale(Vector2::ZeroVector());
	pBaseEnemy_->GetFindSprite()->SetScale(Vector2::ZeroVector());

	spriteEase_.Init("EnemyOverLookSpriteScaling.json");
    spriteEase_.SetAdaptValue(&tempSpriteScale_);
    spriteEase_.Reset();


	isChase_ = true;
}

EnemyWait::~EnemyWait() {

}

void EnemyWait::Update() {

	spriteEase_.Update(Frame::DeltaTimeRate());
	pBaseEnemy_->GetNotFindSprite()->SetScale(tempSpriteScale_);
	
		// ターゲットへのベクトル
		Vector3 direction =pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

		// 距離
		distance_ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

		// 近すぎる場合は追従を停止して攻撃
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
		pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle, 0.3f));

}

void EnemyWait::Debug() {
	
		
}
