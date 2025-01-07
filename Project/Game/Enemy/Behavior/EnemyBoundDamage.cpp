
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
		// Yに加算
		pBaseEnemy_->AddPosition(Vector3(0, speed_, 0));

		// 加速する
		speed_ = max(speed_ - (gravity_ * Frame::DeltaTimeRate()),fallSpeedLimit_);
	
		if (pBaseEnemy_->GetTransform().translation_.y > pBaseEnemy_->InitY_) break;

		step_ = Step::RETUNROOT;
	
		break;
	case Step::RETUNROOT:
	/// -------------------------------------------------------
	/// 追従に戻す
	///---------------------------------------------------------
		pBaseEnemy_->SetColor(Vector4(1.0f, 1, 1, 1.0f));
		pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyChasePlayer>(pBaseEnemy_));
		break;
	}
}

void EnemyBoundDamage::Debug() {
	

}

