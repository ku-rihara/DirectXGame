/// behavior
#include"ComboAttackFirst.h"
#include"ComboAttackRoot.h"
#include"ComboAttackForth.h"

/// objs
#include"Player/Player.h"

/// frame
#include"Frame/Frame.h"

//初期化
ComboAttackForth::ComboAttackForth(Player* player)
	: BaseComboAattackBehavior("ComboAttackForth", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	///collision
	stopCollisionBox_=std::make_unique<StopCollisionBox>();
	thrustCollisionBox_ = std::make_unique<ThrustCollisionBox>();

	// stop
	stopCollisionBox_->Init();
	stopRailManager_ = pPlayer_->GetRightHand()->GetStopRailManager();

	/// trust
	thrustCollisionBox_->Init();
	thrustRailManager_ = pPlayer_->GetLeftHand()->GetThrustRailManager();
	
	order_ = Order::RPUNCH; // 振る舞い順序初期化
}

ComboAttackForth::~ComboAttackForth() {

}

//更新
void ComboAttackForth::Update() {

	stopCollisionBox_->Update();
	thrustCollisionBox_->Update();

	switch (order_) {

	case Order::RPUNCH:
		///----------------------------------------------------
		/// 右パンチ
		///----------------------------------------------------
		
		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailForthComboUpdate(pPlayer_->GetRightHand()->GetRailRunSpeedForth());

		// イージング終了時の処理
		if (stopRailManager_->GetRailMoveTime() < 1.0f) break;

		stopRailManager_->SetRailMoveTime(1.0f);
		order_ = Order::LPUNCH;

		break;

	case Order::RBACKPUNCH:
		///----------------------------------------------------
		/// 右パンチ
		///----------------------------------------------------

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailForthComboUpdate(pPlayer_->GetRightHand()->GetRailRunSpeedForth());

		// イージング終了時の処理
		if (stopRailManager_->GetRailMoveTime() < 1.0f) break;

		stopRailManager_->SetRailMoveTime(1.0f);
		order_ = Order::LPUNCH;

		break;

	case Order::LPUNCH:
		///----------------------------------------------------
		/// 左パンチ
		///----------------------------------------------------


		// レール更新と座標反映
		pPlayer_->GetLeftHand()->RailForthComboUpdate(pPlayer_->GetLeftHand()->GetRailRunSpeedForth());

		// イージング終了時の処理
		if (thrustRailManager_->GetRailMoveTime() < 1.0f) break;

		thrustRailManager_->SetRailMoveTime(1.0f);
		order_ = Order::LBACKPUNCH;


		break;


	case Order::LBACKPUNCH:
		///----------------------------------------------------
		/// バックパンチ
		///----------------------------------------------------
	
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