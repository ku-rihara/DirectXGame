/// behavior
#include"JumpRush.h"
#include"ComboAttackRoot.h"
#include"ThrustPunch.h"
#include<numbers>
/// objs
#include"Player/Player.h"

// input
#include"JoyState/JoyState.h"

/// math
#include"Frame/Frame.h"

//初期化
JumpRush::JumpRush(Player* player)
	: BaseComboAattackBehavior("JumpRush", player) {
	
	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	/// parm
	/*assaultEase_.time = 0.0f;
	assaultEase_.maxTime = pPlayerParameter_->GetNormalComboParm(THIRD).attackEaseMax;*/

	assaultJumpEase_.Init("PlayerAssaultJump");
    assaultJumpEase_.SetAdaptValue(&tempPositionY_);
    assaultJumpEase_.Reset();


	assaultEase_.Init("PlayerAssault");
    assaultEase_.SetAdaptValue(&tempPosition_);
    assaultEase_.Reset();

    assaultEase_.SetOnFinishCallback([this]() {
        order_ = Order::NEXTBEHAVIOR;
    });


	//　モーション
	BaseComboAattackBehavior::AnimationInit();

	// 振る舞い順序初期化
	order_ = Order::CALUCRATION;
}

JumpRush::~JumpRush() {

}

//更新
void JumpRush::Update() {
	BaseComboAattackBehavior::RotateMotionUpdate(0, GetRotateValueAnti(), true);
	BaseComboAattackBehavior::ScalingEaseUpdate();
	

	switch (order_) {

		///----------------------------------------------------
		/// 計算
		///----------------------------------------------------
	case Order::CALUCRATION:
		initPos_ = pPlayer_->GetWorldPosition();
		forwardDirection_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());

		frontPos_ = initPos_ + (forwardDirection_* pPlayerParameter_->GetParamaters().upperParam.BackLashValue);
		
		//easing set
        assaultJumpEase_.SetStartValue(initPos_.y);
        assaultJumpEase_.SetEndValue(initPos_.y + pPlayerParameter_->GetParamaters().upperPosY);

		assaultEase_.SetStartValue(initPos_);
        assaultEase_.SetEndValue(frontPos_);

		order_ = Order::RUSH;
		break;

		///----------------------------------------------------
		/// 待機
		///----------------------------------------------------
	case Order::RUSH:
        assaultJumpEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->SetWorldPositionY(tempPositionY_);

		assaultEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->SetWorldPositionX(tempPosition_.x);
        pPlayer_->SetWorldPositionZ(tempPosition_.z);

		break;

	case Order::NEXTBEHAVIOR:
		pPlayer_->ChangeComboBehavior(std::make_unique<ThrustPunch>(pPlayer_));
		
		break;
	}

}

void JumpRush::Debug() {

}

