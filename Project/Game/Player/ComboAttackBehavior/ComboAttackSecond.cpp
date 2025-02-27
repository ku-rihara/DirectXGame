/// behavior
#include"ComboAttackFirst.h"
#include"ComboAttackSecond.h"
#include"ComboAttackRoot.h"
#include"ComboAttackThird.h"

/// objs
#include"Player/Player.h"

/// input
#include"JoyState/JoyState.h"

/// frame
#include"Frame/Frame.h"

//初期化
ComboAttackSecond::ComboAttackSecond(Player* player)
	: BaseComboAattackBehavior("ComboAttackSecond", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	/// parm
	punchEase_.time = 0.0f;
	waitTine_ = 0.0f;

	/// collisionBox
	collisionBox_ = std::make_unique<AttackCollisionBox>();
	collisionBox_->Init();
	collisionBox_->attackType_ = AttackCollisionBox::AttackType::NORMAL;
	collisionBox_->SetSize(Vector3::UnitVector() * 2.5f);// 当たり判定サイズ
	Vector3 forwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	collisionBox_->SetOffset(forwardDirection * 1.0f);
	collisionBox_->IsAdapt(false);

	/// パンチ座標セット
	lHandStartPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
	lHandTargetPos_ = pPlayer_->GetLeftHand()->GetTransform().LookAt(Vector3::ToForward()) * pPlayer_->GetNormalComboParm(Player::ComboNum::SECOND).attackReach;

	//　モーション
	BaseComboAattackBehavior::AnimationInit();

	pPlayer_->SoundPunch();
	// 振る舞い順序初期化
	order_ = Order::PUNCH;
}

ComboAttackSecond::~ComboAttackSecond() {

}

//更新
void ComboAttackSecond::Update() {

	//　モーション
	BaseComboAattackBehavior::RotateMotionUpdate(0, GetRotateValue(), true);
	BaseComboAattackBehavior::FloatAnimationUpdate();
	/// スケール変化
	BaseComboAattackBehavior::ScalingEaseUpdate();

	pPlayer_->Move(pPlayer_->GetParamater().moveSpeed);


	switch (order_) {

	case Order::PUNCH:

		///----------------------------------------------------
		/// パンチ
		///----------------------------------------------------
		collisionBox_->IsAdapt(true);
		punchEase_.time += Frame::DeltaTimeRate();

		/// 拳を突き出す
		punchPosition_ =
			EaseInSine(lHandStartPos_, lHandTargetPos_, punchEase_.time, pPlayer_->GetNormalComboParm(Player::ComboNum::SECOND).attackEaseMax);


		// ハンドのローカル座標を更新
		pPlayer_->GetLeftHand()->SetWorldPosition(punchPosition_);

		// イージング終了時の処理
		if (punchEase_.time >= pPlayer_->GetNormalComboParm(Player::ComboNum::SECOND).attackEaseMax) {
			punchEase_.time = pPlayer_->GetNormalComboParm(Player::ComboNum::SECOND).attackEaseMax;
			order_ = Order::BACKPUNCH;
		}
		collisionBox_->SetPosition(pPlayer_->GetLeftHand()->GetWorldPosition());
		collisionBox_->Update();

		break;

	case Order::BACKPUNCH:
		///----------------------------------------------------
		/// バックパンチ
		///----------------------------------------------------
		collisionBox_->IsAdapt(false);
		pPlayer_->AdaptRotate();
		punchEase_.time -= Frame::DeltaTimeRate();

		punchPosition_ =
			EaseInSine(lHandStartPos_, lHandTargetPos_, punchEase_.time, pPlayer_->GetNormalComboParm(Player::ComboNum::SECOND).attackEaseMax);

		// ハンドのローカル座標を更新
		pPlayer_->GetLeftHand()->SetWorldPosition(punchPosition_);

		/// ボタンで次のコンボ
		BaseComboAattackBehavior::PreOderNextComboForButton();//コントローラジャンプ	

		// イージング終了時の処理
		if (punchEase_.time > 0.0f) break;
			punchEase_.time = 0.0f;
			order_ = Order::WAIT;
		
		break;

	case Order::WAIT:
		waitTine_ += Frame::DeltaTime();
		pPlayer_->AdaptRotate();

		/// コンボ途切れ
		if (waitTine_ >= pPlayer_->GetNormalComboParm(Player::ComboNum::SECOND).waitTime) {
		
			pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
		}
		else 
		{
			BaseComboAattackBehavior::PreOderNextComboForButton();
			if (isNextCombo_) {
				BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<ComboAttackThird>(pPlayer_));
			}
		}
		break;
	}

}

void ComboAttackSecond::Debug() {

}
