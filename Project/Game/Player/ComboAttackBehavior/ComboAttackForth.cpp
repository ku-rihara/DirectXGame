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

	firstWaitTimeMax_ = 0.2f;

	///collision
	stopCollisionBox_=std::make_unique<StopCollisionBox>();
	thrustCollisionBox_ = std::make_unique<ThrustCollisionBox>();

	// stop
	stopCollisionBox_->Init();
	stopCollisionBox_->SetSize(Vector3::UnitVector() * 3);// 当たり判定サイズ
	stopCollisionBox_->SetPosition(pPlayer_->GetWorldPosition());
	Vector3 sforwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	stopCollisionBox_->SetOffset(sforwardDirection * 5.0f);
	stopCollisionBox_->IsAdapt(true);

	stopRailManager_ = pPlayer_->GetRightHand()->GetStopRailManager();
	stopRailManager_->SetIsRoop(false);

	/// trust
	thrustCollisionBox_->Init();
	thrustCollisionBox_->SetSize(Vector3::UnitVector() * 3);// 当たり判定サイズ
	thrustCollisionBox_->SetPosition(pPlayer_->GetWorldPosition());
	Vector3 tforwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	thrustCollisionBox_->SetOffset(tforwardDirection * 5.0f);
	thrustCollisionBox_->IsAdapt(false);

	thrustRailManager_ = pPlayer_->GetLeftHand()->GetThrustRailManager();
	thrustRailManager_->SetIsRoop(false);

	order_ = Order::FIRSTWAIT; // 振る舞い順序初期化
}

ComboAttackForth::~ComboAttackForth() {

}

//更新
void ComboAttackForth::Update() {

	stopCollisionBox_->Update();
	thrustCollisionBox_->Update();

	switch (order_) {
	case Order::FIRSTWAIT:
		///----------------------------------------------------
		/// 最初の硬直
		///----------------------------------------------------
		firstWaitTime_ += Frame::DeltaTime();
		if (firstWaitTime_ >= firstWaitTimeMax_) {
			order_ = Order::RPUNCH;
		}
		break;

	case Order::RPUNCH:
		///----------------------------------------------------
		/// 右パンチ
		///----------------------------------------------------
		
		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailForthComboUpdate(pPlayer_->GetRightHand()->GetRailRunSpeedForth());

		// イージング終了時の処理
		if (stopRailManager_->GetRailMoveTime() < 1.0f) break;

		stopRailManager_->SetRailMoveTime(1.0f);
		pPlayer_->GetRightHand()->RailForthComboUpdate(0.0f);

		stopCollisionBox_->IsAdapt(false);
		thrustCollisionBox_->IsAdapt(true);
		order_ = Order::RBACKPUNCH;

		break;

	case Order::RBACKPUNCH:
		///----------------------------------------------------
		/// 右パンチ(戻る)
		///----------------------------------------------------

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailForthComboUpdate(-pPlayer_->GetRightHand()->GetRailRunSpeedForth());

		// イージング終了時の処理
		if (stopRailManager_->GetRailMoveTime() > 0.0f) break;

		stopRailManager_->SetRailMoveTime(0.0f);
		pPlayer_->GetRightHand()->RailForthComboUpdate(0.0f);
	

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
		pPlayer_->GetLeftHand()->RailForthComboUpdate(0.0f);
		order_ = Order::LBACKPUNCH;
		thrustCollisionBox_->IsAdapt(false);

		break;


	case Order::LBACKPUNCH:
		///----------------------------------------------------
		/// 左パンチ(戻る)
		///----------------------------------------------------
		
		// レール更新と座標反映
		pPlayer_->GetLeftHand()->RailForthComboUpdate(-pPlayer_->GetLeftHand()->GetRailRunSpeedForth());

		// イージング終了時の処理
		if (thrustRailManager_->GetRailMoveTime() > 0.0f) break;

		thrustRailManager_->SetRailMoveTime(0.0f);
		pPlayer_->GetLeftHand()->RailForthComboUpdate(0.0f);
		

		order_ = Order::WAIT;

		break;

	case Order::WAIT:
	
		waitTine_ += Frame::DeltaTime();
		pPlayer_->Fall();

		if (pPlayer_->GetWorldPosition().y > pPlayer_->InitY_)break;
		if (waitTine_ < pPlayer_->GetWaitTime(Player::FORTH)) break;


			pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));	
	}

}

void ComboAttackForth::Debug() {

}