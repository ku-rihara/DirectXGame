/// behavior
#include"ComboAttackFirst.h"
#include"ComboAttackRoot.h"
#include"ComboAttackForth.h"

/// objs
#include"Player/Player.h"

/// input
#include"input/Input.h"

/// frame
#include"Frame/Frame.h"

//初期化
ComboAttackForth::ComboAttackForth(Player* player)
	: BaseComboAattackBehavior("ComboAttackForth", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	/// parm
	punchEase_.time = 0.0f;
	waitTine_ = 0.0f;

	/// collisionBox
	collisionBox_ = std::make_unique<PunchCollisionBox>();
	collisionBox_->Init();

	Vector3 collisionSize = Vector3::UnitVector();
	collisionBox_->SetSize(collisionSize);// 当たり判定サイズ
	collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
	Vector3 forwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	collisionBox_->SetOffset(forwardDirection * 4.0f);
	collisionBox_->IsAdapt(true);

	/// パンチ座標セット
	lHandStartPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
	lHandTargetPos_ = pPlayer_->GetLeftHand()->GetTransform().LookAt(Vector3::ToForward()) * pPlayer_->GetPunchReach(Player::SECOND);


	// 振る舞い順序初期化
	order_ = Order::PUNCH;
}

ComboAttackForth::~ComboAttackForth() {

}

//更新
void ComboAttackForth::Update() {

	collisionBox_->Update();

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
		collisionBox_->IsAdapt(false);
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
		
	}

}

void ComboAttackForth::Debug() {

}