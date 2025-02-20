
/// behavior
#include"EnemyBoundDamage.h"
#include"EnemyChasePlayer.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"


//初期化
EnemyBoundDamage::EnemyBoundDamage(BaseEnemy* boss)
	: BaseEnemyBehaivor("EnemyBoundDamage", boss) {
	
	speed_ = 2.0f;
	gravity_ = 8.8f;
	fallSpeedLimit_ = -5.2f;

	pBaseEnemy_->DamageEmit();
	pBaseEnemy_->GetNotFindSprite()->SetScale(Vector2(0, 0));
	pBaseEnemy_->GetFindSprite()->SetScale(Vector2(1, 1));
	pBaseEnemy_->SetScale(BaseEnemy::InitScale_);

	step_ = Step::BOUND; /// ステップ初期化
}

EnemyBoundDamage::~EnemyBoundDamage() {

}

void EnemyBoundDamage::Update() {

	
	switch (step_)
	{

	case Step::BOUND:

	/// ------------------------------------------------------
	/// ヒットバッグ
	///---------------------------------------------------------
		pBaseEnemy_->ThrustEmit();
		// Yに加算
		pBaseEnemy_->AddPosition(Vector3(0, speed_, 0));

		// 回転
		rotate_ += pBaseEnemy_->GetParamater().thrustRotateSpeed * Frame::DeltaTimeRate();
		pBaseEnemy_->SetBodyRotateX(rotate_);

		// 加速する
		speed_ = max(speed_ - (gravity_ * Frame::DeltaTimeRate()),fallSpeedLimit_);
	
		if (pBaseEnemy_->GetTransform().translation_.y > pBaseEnemy_->GetParamater().basePosY) break;

		step_ = Step::RETUNROOT;
	
		break;
	case Step::RETUNROOT:
	/// -------------------------------------------------------
	/// 追従に戻す
	///---------------------------------------------------------
		pBaseEnemy_->SetBodyColor(Vector4(1.0f, 1, 1, 1.0f));
		pBaseEnemy_->SetBodyRotateX(0.0f);
		pBaseEnemy_->SetWorldPositionY(pBaseEnemy_->GetParamater().basePosY);
		pBaseEnemy_->BackToDamageRoot();
		break;
	}
}

void EnemyBoundDamage::Debug() {
	

}

