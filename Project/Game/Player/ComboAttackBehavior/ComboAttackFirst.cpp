/// behavior
#include"ComboAttackFirst.h"


/// objs
#include"Player/Player.h"
#include"LockOn/LockOn.h"

/// math
#include"MathFunction.h"
#include"Frame/Frame.h"

#include<imgui.h>


//初期化
ComboAttackFirst::ComboAttackFirst(Player* player)
	: BaseComboAattackBehavior("ComboAttackFirst", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	///　突進の動き
	initPos_ = pPlayer_->GetWorldPosition();
	forwardDirection_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());

	// ハンド攻撃
	rHandStartPos_ = pPlayer_->GetRightHand()->GetTransform().translation_;
	rHandTargetPos_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());//* ×速度

	rushEaseT_ = 0.0f;
	speed_ = pPlayer_->GetRushSpeed();

	// 振る舞い順序初期化
	order_ = Order::RUSH;

}

ComboAttackFirst::~ComboAttackFirst() {

}

//更新
void ComboAttackFirst::Update() {
	switch (order_){
	
	case Order::RUSH:
		///----------------------------------------------------
		/// 突進
		///----------------------------------------------------

		ChangeSpeedForLockOn();// ロックオンによる突進スピードの変化
		rushPos_ = initPos_+(forwardDirection_ * speed_); // 突進座標を決める

		rushEaseT_ += Frame::DeltaTime();

		// 突進の動き
		pPlayer_->SetWorldPosition(
			Lerp(initPos_,rushPos_, rushEaseT_));

		/// パンチオーダーに移行
		if (rushEaseT_ >= 1.0f) {
			rushEaseT_ = 1.0f;
			order_ = Order::PUNCH;
		}

		break;
	case Order::PUNCH:
		break;
	default:
		break;
	}

}

void ComboAttackFirst::ChangeSpeedForLockOn() {
	// ターゲット追い越し防止
	if (!(pPlayer_->GetLockOn() &&
		pPlayer_->GetLockOn()->GetEnemyTarget())) {
		return;
	}

	Vector3 differectialVector = pPlayer_->GetLockOn()->GetTargetPosition() - pPlayer_->GetWorldPosition();
	// 距離
	float distance = differectialVector.Length();
	// 距離しきい値
	const float threshold = 0.2f;

	// しきい値より離れていない場合は終了
	if (distance <= threshold) {
		return;
	}

	// Y軸回り角度
	pPlayer_->SetRotationY(std::atan2(differectialVector.x, differectialVector.z));
	// しきい値を超える速さなら補正する
	if (speed_ > distance - threshold) {
		speed_ = distance - threshold;
	}

}

void  ComboAttackFirst::Debug() {
	ImGui::Text("ComboAttackFirst");
}