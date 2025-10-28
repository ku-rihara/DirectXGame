
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

	pBaseEnemy_->DamageRenditionInit();

	///
	pBaseEnemy_->GetNotFindSprite()->SetScale(Vector2(0, 0));
	pBaseEnemy_->GetFindSprite()->SetScale(Vector2(1, 1));
    pBaseEnemy_->SetScale(pBaseEnemy_->GetParameter().initScale_);

	archingRotateEase_.Init("EnemyArchingRotate.json");
    archingRotateEase_.SetAdaptValue(&tempArchingRotateX_);
    archingRotateEase_.Reset();

   
	archingPosEase_.Init("EnemyArchingPos.json");
    archingPosEase_.SetAdaptValue(&tempArchingPos_);
    archingPosEase_.Reset();

    archingRotateEase_.SetOnFinishCallback([this]() {
        step_ = Step::RETUNROOT;
    });


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

		archingPosEase_.SetStartValue(initPos_);
        archingPosEase_.SetEndValue(backPos_);

		step_ = Step::HITBACK;
		break;

	/// ------------------------------------------------------
	/// ヒットバッグ
	///---------------------------------------------------------
	case Step::HITBACK:

		// 最短角度補間でプレイヤーの回転を更新
		pBaseEnemy_->SetRotationY(LerpShortAngle(pBaseEnemy_->GetTransform().rotation_.y, objectiveAngle_, 0.5f));

		
		//のけぞり適応
        archingRotateEase_.Update(Frame::DeltaTimeRate());
		pBaseEnemy_->SetBodyRotateX(tempArchingRotateX_);

		 archingPosEase_.Update(Frame::DeltaTimeRate());
        pBaseEnemy_->SetWorldPosition(tempArchingPos_);

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
