/// behavior
#include"ComboAttackJumpSecond.h"

/// Player
#include"Player/Player.h"

#include<imgui.h>
#include"Frame/Frame.h"

//初期化
ComboAttackJumpSecond::ComboAttackJumpSecond(Player* player)
	: BaseComboAattackBehavior("ComboAttackJumpSecond", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------
	handMoveEasing_.maxTime = 0.2f;

	///初期化座標とターゲット座標
	initRHandPos_ = pPlayer_->GetRightHand()->GetTransform().translation_;
	initLHandPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
	targetRPos_ = initRHandPos_ +(Vector3::ToForward()*2.0f);
	targetLPos_ = initLHandPos_ +(Vector3::ToForward()*2.0f);

	initPos_ = pPlayer_->GetWorldPosition();
	direction_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	rushTargetPos_ = initPos_ + (direction_ * pPlayer_->GetJPunchReach(Player::JSECOND));
	
	rushEaseTime_ = 0.0f;

	step_ = STEP::RUSH; // 突進

}

ComboAttackJumpSecond::~ComboAttackJumpSecond() {

}

//更新
void ComboAttackJumpSecond::Update() {
	switch (step_)
	{

	case STEP::RUSH:
		///---------------------------------------------------------
		/// 着地
		///---------------------------------------------------------
		handMoveEasing_.time += Frame::DeltaTimeRate();
		rushEaseTime_ += Frame::DeltaTimeRate();

		/// ハンド
		handMoveEasing_.time = std::min(handMoveEasing_.time, handMoveEasing_.maxTime);

		pPlayer_->GetRightHand()->SetWorldPosition(
			EaseInExpo(initRHandPos_,targetRPos_, handMoveEasing_.time, handMoveEasing_.maxTime)
		);

		pPlayer_->GetLeftHand()->SetWorldPosition(
			EaseInExpo(initLHandPos_, targetLPos_, handMoveEasing_.time, handMoveEasing_.maxTime)
		);

		/// 突進
		pPlayer_->SetWorldPosition(
			EaseOutQuart(initPos_, rushTargetPos_, rushEaseTime_, pPlayer_->GetJPunchEaseMax(Player::JSECOND))
		);

		// 早期break
		if (rushEaseTime_ < pPlayer_->GetJPunchEaseMax(Player::JSECOND))break;
		step_ = STEP::WAIT;

		break;
	case STEP::WAIT:
		///---------------------------------------------------------
		/// 待機
		///---------------------------------------------------------


		break;
	case STEP::RETURNROOT:
		BaseComboAattackBehavior::ChangeNextComboFragForButton();//次のコンボに移行可能
		/*BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<>)*/
		break;
	default:
		break;
	}

}


void  ComboAttackJumpSecond::Debug() {
	ImGui::Text("ComboAttackJumpSecond");
}