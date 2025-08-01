/// behavior
#include"EnemyStopDamage.h"
#include"EnemyChasePlayer.h"
/// obj
#include"Enemy/BaseEnemy.h"
#include"Player/Player.h"
/// math
#include"MathFunction.h"
#include"Frame/Frame.h"

//初期化
EnemyStopDamage::EnemyStopDamage(BaseEnemy* boss)
	: BaseEnemyBehavior("EnemyStopDamage", boss) {

	// 赤色
	pBaseEnemy_->SetBodyColor(Vector4(0.9f, 0, 0, 0.9f));
	kStopTime_ = 1.0f;

	pBaseEnemy_->DamageRenditionInit();
}

EnemyStopDamage::~EnemyStopDamage() {

}

void EnemyStopDamage::Update() {

	stopTime_ += Frame::DeltaTime();
	if (stopTime_ < kStopTime_) return;
	
	// Yに加算
	pBaseEnemy_->AddPosition(Vector3(0, speed_, 0));

	// 加速する
	speed_ = max(speed_ - (4.8f * Frame::DeltaTimeRate()),-1.3f);
	
	// 着地
	if (pBaseEnemy_->GetTransform().translation_.y > pBaseEnemy_->GetParameter().basePosY) return;
	pBaseEnemy_->SetWorldPositionY(pBaseEnemy_->GetParameter().basePosY);

	/// -------------------------------------------------------
	/// 追従に戻す
	///---------------------------------------------------------
	pBaseEnemy_->BackToDamageRoot();

}

void EnemyStopDamage::Debug() {


}
