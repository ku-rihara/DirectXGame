
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
	: BaseEnemyBehavior("EnemyHitBackDamage", boss) {

	/// ヒットバックのパラメータ
	initPos_ = pBaseEnemy_->GetWorldPosition();
	speed_ = 0.5f;
	// 赤色
	pBaseEnemy_->SetBodyColor(Vector4(0.9f, 0, 0, 0.9f));

	easing_.time = 0.0f;
	easing_.maxTime = 0.2f;

	pBaseEnemy_->DamageRenditionInit();

	///
	pBaseEnemy_->GetNotFindSprite()->SetScale(Vector2(0, 0));
	pBaseEnemy_->GetFindSprite()->SetScale(Vector2(1, 1));
	pBaseEnemy_->SetScale(BaseEnemy::InitScale_);

	/// hitbackMove
	archingbackEase_.time = 0.0f;
	archingbackEase_.maxTime=pBaseEnemy_->GetParamater().archingbackTime;
	archingbackEase_.backRatio= pBaseEnemy_->GetParamater().archingbackRatio;

	step_ = Step::DIRECTIONSET; /// ステップ初期化
}

EnemyHitBackDamage::~EnemyHitBackDamage() {

}

void EnemyHitBackDamage::Update() {
	
	switch (step_)
	{

	/// ------------------------------------------------------
	/// 向き計算
	///---------------------------------------------------------
	case Step::DIRECTIONSET:
		AngleCaluclation();
		
		/// 吹っ飛ぶ距離を計算
		backPos_ = initPos_ + (direction_ * -speed_);

		step_ = Step::HITBACK;
		break;

	/// ------------------------------------------------------
	/// ヒットバッグ
	///---------------------------------------------------------
	case Step::HITBACK:

		// 最短角度補間でプレイヤーの回転を更新
		pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle_, 0.5f));

		// タイム更新
		easing_.time += Frame::DeltaTimeRate();
		archingbackEase_.time += Frame::DeltaTimeRate();
		//のけぞり
		preRotate_ = Back::OutCircZero(0.0f, pBaseEnemy_->GetParamater().archingbackValue, archingbackEase_.time, archingbackEase_.maxTime,archingbackEase_.backRatio);
		//のけぞり適応
		pBaseEnemy_->SetBodyRotateX(preRotate_);

		easing_.time = std::min(easing_.time, easing_.maxTime);
		/// イージングでヒットバックする
		pBaseEnemy_->SetWorldPosition(
			EaseInSine(initPos_, backPos_, easing_.time, easing_.maxTime)
		);


		//次のステップ	
		if (archingbackEase_.time < archingbackEase_.maxTime) break;
			archingbackEase_.time = archingbackEase_.maxTime;
			step_ = Step::RETUNROOT;
		

		break;
	/// -------------------------------------------------------
	/// 通常に戻す
	///---------------------------------------------------------
	case Step::RETUNROOT:
	
		pBaseEnemy_->SetBodyColor(Vector4(1.0f, 1, 1, 1.0f));
		pBaseEnemy_->RotateInit();
		pBaseEnemy_->BackToDamageRoot();
		break;

	}
	
}

void EnemyHitBackDamage::Debug() {
	

}
