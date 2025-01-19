
/// behavior
#include"EnemyRoot.h"
#include"EnemyChasePlayer.h"

/// math
#include"MathFunction.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"/// プレイヤーの座標取得に使ってるので消さないこと
/// frame
#include"Frame/Frame.h"
/// imgui
#include<imgui.h>
/// std
#include<cmath> 

//初期化
EnemyRoot::EnemyRoot(BaseEnemy* boss)
	: BaseEnemyMoveBehavior("EnemyRoot", boss) {
	

	//パラメータ初期化
	chaseSpeedNormal_ = 0.2f;
	waveAttackStartPos_ = 25.0f;/// ボス
	normalAttackStartPos_ = 7.0f;
	attackCoolTime_ = 1.0f;

	pBaseEnemy_->GetNotFindSprite()->SetScale(Vector2(0, 0));
	pBaseEnemy_->GetFindSprite()->SetScale(Vector2(0, 0));
	spriteEase_.maxTime = 1.0f;
	spriteEase_.time = 0.0f;

	isChase_ = true;//	デバッグ用
}

EnemyRoot::~EnemyRoot() {

}

void EnemyRoot::Update() {

	spriteEase_.time += Frame::DeltaTime();
	spriteEase_.time = std::min(spriteEase_.time, spriteEase_.maxTime);
	pBaseEnemy_->GetNotFindSprite()->SetScale(EaseOutBack(Vector2(0,0), Vector2(1, 1), spriteEase_.time, spriteEase_.maxTime));
	
		// ターゲットへのベクトル
		Vector3 direction =pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

		// 距離
		distance_ = std::sqrt(direction.x * direction.x + direction.z * direction.z);

		// 近すぎる場合は追従を停止して攻撃
		if (distance_ < pBaseEnemy_->GetParamater().chaseDistance) {
			pBaseEnemy_->ChangeMoveBehavior(std::make_unique<EnemyChasePlayer>(pBaseEnemy_));
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

void EnemyRoot::Debug() {
	
		
}
