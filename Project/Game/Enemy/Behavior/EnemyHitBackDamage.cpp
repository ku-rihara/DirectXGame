
/// behavior
#include"EnemyHitBackDamage.h"
#include"EnemyChasePlayer.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"

//初期化
EnemyHitBackDamage::EnemyHitBackDamage(BaseEnemy* boss)
	: BaseEnemyBehaivor("EnemyHitBackDamage", boss) {

	/// ヒットバックのパラメータ
	initPos_ = pBaseEnemy_->GetWorldPosition();
	speed_ = 0.01f;
	// 赤色
	pBaseEnemy_->SetColor(Vector4(0.9f, 0, 0, 0.9f));

	easing_.time = 0.0f;
	easing_.maxTime = 0.01f;

	pBaseEnemy_->DamageEmit();

	pBaseEnemy_->GetNotFindSprite()->SetScale(Vector2(0, 0));
	pBaseEnemy_->GetFindSprite()->SetScale(Vector2(1, 1));
	pBaseEnemy_->SetScale(BaseEnemy::InitScale_);


	step_ = Step::DIRECTIONSET; /// ステップ初期化
}

EnemyHitBackDamage::~EnemyHitBackDamage() {

}

void EnemyHitBackDamage::Update() {
	
	switch (step_)
	{

	case Step::DIRECTIONSET:
	/// ------------------------------------------------------
	/// 向き計算
	///---------------------------------------------------------

		// ターゲットへのベクトル
		direction_ = pBaseEnemy_->GetDirectionToTarget(pBaseEnemy_->GetPlayer()->GetWorldPosition());

		direction_.y = 0.0f;
		direction_.Normalize();

		// 目標角度を計算
		objectiveAngle_ = std::atan2(-direction_.x, -direction_.z);

		/// 吹っ飛ぶ距離を計算
		backPos_ = initPos_ + (direction_ * -speed_);

		step_ = Step::HITBACK;
		break;

	case Step::HITBACK:

	/// ------------------------------------------------------
	/// ヒットバッグ
	///---------------------------------------------------------

		// 最短角度補間でプレイヤーの回転を更新
		pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle_, 0.5f));

		easing_.time += Frame::DeltaTimeRate();

		/// イージングでヒットバックする
		pBaseEnemy_->SetWorldPosition(
			EaseInSine(initPos_, backPos_, easing_.time, easing_.maxTime)
		);

		//次のステップ	
		if (easing_.time >= easing_.maxTime) {
			easing_.time = easing_.maxTime;
			step_ = Step::RETUNROOT;
		}


		break;
	case Step::RETUNROOT:
	/// -------------------------------------------------------
	/// 通常に戻す
	///---------------------------------------------------------
		pBaseEnemy_->SetColor(Vector4(1.0f, 1, 1, 1.0f));
		pBaseEnemy_->BackToDamageRoot();
		break;

	}
	
}

void EnemyHitBackDamage::Debug() {
	

}
