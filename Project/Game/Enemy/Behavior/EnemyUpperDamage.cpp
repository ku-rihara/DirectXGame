
/// behavior
#include"EnemyUpperDamage.h"
#include"EnemyChasePlayer.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"


//初期化
EnemyUpperDamage::EnemyUpperDamage(BaseEnemy* boss)
	: BaseEnemyBehaivor("EnemyUpperDamage", boss) {

	
	speed_ = pBaseEnemy_->GetParamater().upperJumpPower *60.0f; //1.35
	gravity_ = pBaseEnemy_->GetParamater().upperGravity * 60.0f;//3.5
	kFallSpeedLimit_ = pBaseEnemy_->GetParamater().upperFallSpeedLimit*60.0f;//1.2f
	fallSpeedLimit_ = -0.0f;
	stopTime_ = 0.0f;
	kStopTime_ = 0.3f;
	// 赤色
	pBaseEnemy_->SetBodyColor(Vector4(0.9f, 0, 0, 0.9f));

	pBaseEnemy_->DamageEmit();

	step_ = Step::HITBACK; /// ステップ初期化
}

EnemyUpperDamage::~EnemyUpperDamage() {

}

void EnemyUpperDamage::Update() {
	
	
	switch (step_)
	{

	case Step::HITBACK:

	/// ------------------------------------------------------
	/// ヒットバッグ
	///---------------------------------------------------------

		// ターゲットへのベクトル
		direction_ = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

		direction_.y = 0.0f;
		direction_.Normalize();

		// 目標角度を計算
		objectiveAngle_ = std::atan2(-direction_.x, -direction_.z);

		// 最短角度補間でプレイヤーの回転を更新
		pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle_, 0.5f));

		pBaseEnemy_->Jump(speed_, fallSpeedLimit_, gravity_);

		if (speed_ > 0.0f) break;

			stopTime_ += Frame::DeltaTime();
			if (stopTime_ >= kStopTime_) {
				fallSpeedLimit_ = kFallSpeedLimit_ ;
			}
		

		// 着地
		if (pBaseEnemy_->GetTransform().translation_.y > pBaseEnemy_->GetParamater().basePosY) break;
		// 追従に戻す
			pBaseEnemy_->SetWorldPositionY(pBaseEnemy_->GetParamater().basePosY);
			step_ = Step::RETUNROOT;
		
		break;
	case Step::RETUNROOT:
	/// -------------------------------------------------------
	/// 追従に戻す
	///---------------------------------------------------------
		pBaseEnemy_->SetBodyColor(Vector4(1.0f, 1, 1, 1.0f));
		pBaseEnemy_->BackToDamageRoot();
		break;
	}
}

void EnemyUpperDamage::Debug() {
	

}

