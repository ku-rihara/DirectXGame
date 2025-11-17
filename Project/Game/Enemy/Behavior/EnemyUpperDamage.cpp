
/// behavior
#include"EnemyUpperDamage.h"
#include"EnemyChasePlayer.h"
/// obj
#include"Enemy/Types/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"


//初期化
EnemyUpperDamage::EnemyUpperDamage(BaseEnemy* boss)
	: BaseEnemyBehavior("EnemyUpperDamage", boss) {

	
	speed_ = pBaseEnemy_->GetParameter().upperJumpPower *60.0f; //1.35
	gravity_ = pBaseEnemy_->GetParameter().upperGravity * 60.0f;//3.5
	kFallSpeedLimit_ = pBaseEnemy_->GetParameter().upperFallSpeedLimit*60.0f;//1.2f
	fallSpeedLimit_ = -0.0f;
	stopTime_ = 0.0f;
	kStopTime_ = 0.3f;
	// 赤色
	pBaseEnemy_->SetBodyColor(Vector4(0.9f, 0, 0, 0.9f));
	pBaseEnemy_->RotateInit();
	pBaseEnemy_->DamageRenditionInit();

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

		AngleCaluclation();

		// 最短角度補間でプレイヤーの回転を更新
		pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle_, 0.5f));

		rotate_ += pBaseEnemy_->GetParameter().thrustRotateSpeed * Frame::DeltaTimeRate();
		pBaseEnemy_->SetBodyRotateX(rotate_);

		pBaseEnemy_->Jump(speed_, fallSpeedLimit_, gravity_);

		if (speed_ > 0.0f) break;

			stopTime_ += Frame::DeltaTimeRate();
			if (stopTime_ >= kStopTime_) {
				fallSpeedLimit_ = kFallSpeedLimit_ ;
			}
		

		// 着地
		if (pBaseEnemy_->GetTransform().translation_.y > pBaseEnemy_->GetParameter().basePosY) break;
		// 追従に戻す
			pBaseEnemy_->SetWorldPositionY(pBaseEnemy_->GetParameter().basePosY);
			step_ = Step::RETUNROOT;
		
		break;
	case Step::RETUNROOT:
	/// -------------------------------------------------------
	/// 追従に戻す
	///---------------------------------------------------------
		pBaseEnemy_->RotateInit();
		pBaseEnemy_->SetBodyColor(Vector4(1.0f, 1, 1, 1.0f));
		pBaseEnemy_->BackToDamageRoot();
		break;
	}
}

void EnemyUpperDamage::Debug() {
	

}

