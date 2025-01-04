
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

	
	speed_ = 1.5f; 
	gravity_ = 3.5f;
	fallSpeedLimit_ = -0.0f;
	stopTime_ = 0.0f;
	kStopTime_ = 0.3f;
	// 赤色
	pBaseEnemy_->SetColor(Vector4(0.9f, 0, 0, 0.9f));

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

		// Yに加算
		pBaseEnemy_->AddPosition(Vector3(0, speed_, 0));
		
		// 加速する
		speed_ =max(speed_ - (gravity_ * Frame::DeltaTimeRate()), fallSpeedLimit_);

		if (speed_ > 0.0f) break;

			stopTime_ += Frame::DeltaTime();
			if (stopTime_ >= kStopTime_) {
				fallSpeedLimit_ = -1.2f;
			}
		

		// 着地
		if (pBaseEnemy_->GetTransform().translation_.y > BaseEnemy::InitY_) break;
		// 追従に戻す
			pBaseEnemy_->SetWorldPositionY(BaseEnemy::InitY_);
			step_ = Step::RETUNROOT;
		
		break;
	case Step::RETUNROOT:
	/// -------------------------------------------------------
	/// 追従に戻す
	///---------------------------------------------------------
		pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyChasePlayer>(pBaseEnemy_));
		break;
	}
}

void EnemyUpperDamage::Debug() {
	

}
