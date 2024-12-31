/// behavior
#include"ComboAttackThird.h"
#include"ComboAttackSecond.h"
#include"ComboAttackRoot.h"

/// objs
#include"Player/Player.h"

/// math
#include"Frame/Frame.h"

#include<imgui.h>


//初期化
ComboAttackThird::ComboAttackThird(Player* player)
	: BaseComboAattackBehavior("ComboAttackSecond", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	/// parm
	punchEase_.time = 0.0f;
	waitTine_ = 0.0f;

	/// パンチ座標セット
	lHandStartPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
	lHandTargetPos_ = pPlayer_->GetLeftHand()->GetTransform().LookAt(Vector3::ToForward()) * pPlayer_->GetPunchReach(Player::SECOND);


	railManager_ = pPlayer_->GetRightHand()->GetRailManager();
	railManager_->SetRailMoveTime(0.0f);
	railManager_->SetIsRoop(true);

	// 振る舞い順序初期化
	order_ = Order::UPPER;
}

ComboAttackThird::~ComboAttackThird() {

}

//更新
void ComboAttackThird::Update() {
	switch (order_) {

	case Order::UPPER:

		///----------------------------------------------------
		/// アッパー
		///----------------------------------------------------

		pPlayer_->GetRightHand()->RailUpdate();

		pPlayer_->GetRightHand()->SetWorldPosition(railManager_->GetPositionOnRail());

		// イージング終了時の処理
		if (railManager_->GetRailMoveTime() >= 1.0f) {
			railManager_->SetRailMoveTime(1.0f);
			
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

		/*/// 3コンボ目に移行
		else if (Input::GetInstance()->TrrigerKey(DIK_H)) {
			pPlayer_->ChangeComboBehavior
			(std::make_unique<ComboAttackSecond>(pPlayer_));
		}*/
	}

}

void ComboAttackThird::Debug() {

}