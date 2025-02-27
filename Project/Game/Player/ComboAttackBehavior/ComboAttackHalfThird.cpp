/// behavior
#include"ComboAttackHalfThird.h"
#include"ComboAttackRoot.h"
#include"ComboAttackForth.h"
#include<numbers>
/// objs
#include"Player/Player.h"

// input
#include"JoyState/JoyState.h"

/// math
#include"Frame/Frame.h"

//初期化
ComboAttackHalfThird::ComboAttackHalfThird(Player* player)
	: BaseComboAattackBehavior("ComboAttackHalfThird", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	
	/// parm
	assaultEase_.time = 0.0f;
	assaultEase_.maxTime = pPlayer_->GetNormalComboParm(Player::ComboNum::THIRD).attackEaseMax;


	//　モーション
	BaseComboAattackBehavior::AnimationInit();

	// 振る舞い順序初期化
	order_ = Order::CALUCRATION;


}

ComboAttackHalfThird::~ComboAttackHalfThird() {

}

//更新
void ComboAttackHalfThird::Update() {
	BaseComboAattackBehavior::RotateMotionUpdate(0, GetRotateValueAnti(), true);
	BaseComboAattackBehavior::ScalingEaseUpdate();
	

	switch (order_) {

		///----------------------------------------------------
		/// 計算
		///----------------------------------------------------
	case Order::CALUCRATION:
		initPos_ = pPlayer_->GetWorldPosition();
		forwardDirection_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());

		frontPos_ = initPos_ + (forwardDirection_* pPlayer_->GetParamater().upperParm.BackLashValue);
		
		order_ = Order::RUSH;
		break;

		///----------------------------------------------------
		/// 待機
		///----------------------------------------------------
	case Order::RUSH:
		assaultEase_.time += Frame::DeltaTimeRate();
		assaultEase_.time = std::min(assaultEase_.time, assaultEase_.maxTime);

		//Y
		pPlayer_->SetWorldPositionY(
			EaseInSine(initPos_.y, initPos_.y + pPlayer_->GetParamater().upperPosY, assaultEase_.time, assaultEase_.maxTime));

		//前進
		preWorldPos_ = EaseInCubic(initPos_, frontPos_, assaultEase_.time, assaultEase_.maxTime);
		pPlayer_->SetWorldPositionX(preWorldPos_.x);
		pPlayer_->SetWorldPositionZ(preWorldPos_.z);

		if (assaultEase_.time < assaultEase_.maxTime)break;
		order_ = Order::NEXTBEHAVIOR;

		break;

	case Order::NEXTBEHAVIOR:
		pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackForth>(pPlayer_));
		
		break;
	}

}

void ComboAttackHalfThird::Debug() {

}

