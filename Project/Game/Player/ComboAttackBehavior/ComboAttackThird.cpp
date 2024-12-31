/// behavior
#include"ComboAttackThird.h"
#include"ComboAttackSecond.h"
#include"ComboAttackRoot.h"

/// objs
#include"Player/Player.h"

/// math
#include"Frame/Frame.h"
#include<algorithm>

#include<imgui.h>


//初期化
ComboAttackThird::ComboAttackThird(Player* player)
	: BaseComboAattackBehavior("ComboAttackSecond", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	initPosY_ = pPlayer_->GetWorldPosition().y;

	/// parm
	punchEase_.time = 0.0f;
	waitTine_ = 0.0f;
	upperJumpEase_.time = 0.0f;
	upperJumpEase_.maxTime = 0.1f;
	

	railManager_ = pPlayer_->GetRightHand()->GetRailManager();
	railManager_->SetRailMoveTime(0.0f);
	railManager_->SetIsRoop(false);

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

		upperJumpEase_.time += Frame::DeltaTimeRate();

		upperJumpEase_.time = std::min(upperJumpEase_.time, upperJumpEase_.maxTime);

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailUpdate();
		pPlayer_->GetRightHand()->SetWorldPosition(railManager_->GetPositionOnRail());

		pPlayer_->SetWorldPositionY(
			EaseInSine(initPosY_,initPosY_+pPlayer_->GetUpperPosY(),upperJumpEase_.time,upperJumpEase_.maxTime)
		);

		// イージング終了時の処理
		if (railManager_->GetRailMoveTime() >= 1.0f) {
			railManager_->SetRailMoveTime(1.0f);
		}

		break;

	case Order::BACKPUNCH:
		///----------------------------------------------------
		/// バックパンチ
		///----------------------------------------------------
		

		//// イージング終了時の処理
		//if (punchEase_.time <= 0.0f) {
		//	punchEase_.time = 0.0f;
		//	order_ = Order::WAIT;
		//}
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