
/// behavior
#include"EnemyDamageRoot.h"
#include"EnemyDeath.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"


//初期化
EnemyDeath::EnemyDeath(BaseEnemy* boss)
	: BaseEnemyBehavior("EnemyDeath", boss) {
	
	rotate_ = 0.0f;
	blowTime_ = 0.0f;
	savePos_ = pBaseEnemy_->GetWorldPosition();
	blowJumpValue_ = pBaseEnemy_->GetParamater().blowValueY;
	gravity_ = pBaseEnemy_->GetParamater().blowGravity;//3.5
	kFallSpeedLimit_=60.0f;
	step_ = Step::DIRECTIONSET;
}

EnemyDeath::~EnemyDeath() {

}

void EnemyDeath::Update() {
	switch (step_)
	{

	/// ------------------------------------------------------
	/// 向き計算
	///---------------------------------------------------------
	case Step::DIRECTIONSET:
		AngleCaluclation();
		blowPower_ = Vector3(
			direction_.x * -pBaseEnemy_->GetParamater().blowValue*Frame::DeltaTimeRate(),
			0.0f,
			direction_.z * -pBaseEnemy_->GetParamater().blowValue * Frame::DeltaTimeRate());
		step_ = Step::BLOW;
		break;

	/// -------------------------------------------------------
	/// 通常に戻す
	///---------------------------------------------------------
	case EnemyDeath::Step::BLOW:
		pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle_, 0.5f));
		burstTime_ += Frame::DeltaTimeRate();
	

		// 吹っ飛び回転
		rotate_ += pBaseEnemy_->GetParamater().blowRotateSpeed*Frame::DeltaTimeRate();
		pBaseEnemy_->SetRotationX(rotate_);

		// 吹っ飛び適応
		pBaseEnemy_->AddPosition(blowPower_);
			pBaseEnemy_->Jump(blowJumpValue_, kFallSpeedLimit_, gravity_);


		if (burstTime_ < pBaseEnemy_->GetParamater().burstTime)break
		step_ = Step::BURST;
		break;
	
	/// -------------------------------------------------------
	/// 爆散
	///---------------------------------------------------------
	case EnemyDeath::Step::BURST:
		pBaseEnemy_->DeathEmit();
		step_ = Step::DEATH;
		break;
	
	/// -------------------------------------------------------
	/// 死
	///---------------------------------------------------------
	case EnemyDeath::Step::DEATH:
		pBaseEnemy_->SetIsDeath(true);

		break;
	default:
		break;
	}
}

void EnemyDeath::Debug() {
	

}

