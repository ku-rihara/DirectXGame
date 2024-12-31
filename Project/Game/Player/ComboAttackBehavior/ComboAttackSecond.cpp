/// behavior
#include"ComboAttackFirst.h"
#include"ComboAttackSecond.h"
#include"ComboAttackRoot.h"
#include"ComboAttackThird.h"

/// objs
#include"Player/Player.h"

/// input
#include"input/Input.h"

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
	collisionBox_ = std::make_unique<PunchCollisionBox>();
	collisionBox_->Init();

	/// パンチ座標セット
	lHandStartPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
	lHandTargetPos_ = pPlayer_->GetLeftHand()->GetTransform().LookAt(Vector3::ToForward()) * pPlayer_->GetPunchReach(Player::SECOND);


	// 振る舞い順序初期化
	order_ = Order::PUNCH;
}

ComboAttackSecond::~ComboAttackSecond() {

}

//更新
void ComboAttackSecond::Update() {
	switch (order_) {

	case Order::PUNCH:
		///----------------------------------------------------
		/// パンチ
		///----------------------------------------------------

		punchEase_.time += Frame::DeltaTimeRate();

		/// 拳を突き出す
		punchPosition_ =
			EaseInSine(lHandStartPos_, lHandTargetPos_, punchEase_.time, pPlayer_->GetPunchEaseMax(Player::SECOND));


		// ハンドのローカル座標を更新
		pPlayer_->GetLeftHand()->SetWorldPosition(punchPosition_);

		// イージング終了時の処理
		if (punchEase_.time >= pPlayer_->GetPunchEaseMax(Player::SECOND)) {
			punchEase_.time = pPlayer_->GetPunchEaseMax(Player::SECOND);
			order_ = Order::BACKPUNCH;
		}

		break;

	case Order::BACKPUNCH:
		///----------------------------------------------------
		/// バックパンチ
		///----------------------------------------------------
		punchEase_.time -= Frame::DeltaTimeRate();

		punchPosition_ =
			EaseInSine(lHandStartPos_, lHandTargetPos_, punchEase_.time, pPlayer_->GetPunchEaseMax(Player::SECOND));

		// ハンドのローカル座標を更新
		pPlayer_->GetLeftHand()->SetWorldPosition(punchPosition_);

		// イージング終了時の処理
		if (punchEase_.time <= 0.0f) {
			punchEase_.time = 0.0f;
			order_ = Order::WAIT;
		}
		break;

	case Order::WAIT:
		waitTine_ += Frame::DeltaTime();

		/// コンボ途切れ
		if (waitTine_ >= pPlayer_->GetWaitTime(Player::SECOND)) {
			pPlayer_->ChangeComboBehavior
			(std::make_unique<ComboAttackRoot>(pPlayer_));
		}
		/// 3コンボ目に移行
		else if (Input::GetInstance()->TrrigerKey(DIK_H)) {
			pPlayer_->ChangeComboBehavior
			(std::make_unique<ComboAttackThird>(pPlayer_));
		}
	}

}

void ComboAttackSecond::Debug() {

}