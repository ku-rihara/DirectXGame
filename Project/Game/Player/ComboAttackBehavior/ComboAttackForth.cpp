/// behavior
#include"ComboAttackFirst.h"
#include"ComboAttackRoot.h"
#include"ComboAttackForth.h"
#include"ComboAttackJumpFirst.h"

/// objs
#include"Player/Player.h"
#include"GameCamera/GameCamera.h"

/// frame
#include"Frame/Frame.h"

//初期化
ComboAttackForth::ComboAttackForth(Player* player)
	: BaseComboAattackBehavior("ComboAttackForth", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	firstWaitTimeMax_ = 0.2f;
	pPlayer_->SetHeadRotateX(0.0f);

	///collision
	stopCollisionBox_=std::make_unique<StopCollisionBox>();
	thrustCollisionBox_ = std::make_unique<ThrustCollisionBox>();

	// stop
	stopCollisionBox_->Init();
	stopCollisionBox_->SetSize(Vector3::UnitVector() * 4.5f);// 当たり判定サイズ
	Vector3 sforwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	stopCollisionBox_->SetOffset(sforwardDirection * 2.0f);
	stopCollisionBox_->SetPosition(pPlayer_->GetRightHand()->GetWorldPosition());
	stopCollisionBox_->IsAdapt(false);
	stopCollisionBox_->Update();

	stopRailManager_ = pPlayer_->GetRightHand()->GetStopRailManager();
	stopRailManager_->SetIsRoop(false);

	/// trust
	thrustCollisionBox_->Init();
	thrustCollisionBox_->SetSize(Vector3::UnitVector() * 4.5f);// 当たり判定サイズ
	thrustCollisionBox_->SetPosition(pPlayer_->GetWorldPosition());
	Vector3 tforwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	thrustCollisionBox_->SetOffset(tforwardDirection * 2.0f);
	thrustCollisionBox_->IsAdapt(false);
	thrustCollisionBox_->Update();

	thrustRailManager_ = pPlayer_->GetLeftHand()->GetThrustRailManager();
	thrustRailManager_->SetIsRoop(false);

	///land
	startEasing_.maxTime = 0.5f;
	startEasing_.amplitude = 0.6f;
	startEasing_.period = 0.2f;

	kTimeDownTime_ = 1.1f;
	timeDownTime_ = 0.0f;
	istimeSlow_ = false;

	order_ = Order::FIRSTWAIT; // 振る舞い順序初期化
	fallInitSpeed_ = 0.0f;
}

ComboAttackForth::~ComboAttackForth() {

}

//更新
void ComboAttackForth::Update() {
	/// スケール変化
	startEasing_.time += Frame::DeltaTimeRate();
	startEasing_.time = std::min(startEasing_.time, startEasing_.maxTime);
	pPlayer_->SetScale(EaseAmplitudeScale(Vector3::UnitVector(), startEasing_.time, startEasing_.maxTime,
		startEasing_.amplitude, startEasing_.period));

	/*pPlayer_->Move(0.01f);*/

	//デルタタイムスケール小さく
	if (thrustCollisionBox_->GetIsSlow() && !istimeSlow_) {
		Frame::SetTimeScale(0.01f);
		pPlayer_->GetGameCamera()->ChangeZoomInOut();
		istimeSlow_ = true;
	}


	// スロータイム
	if (istimeSlow_) {
		timeDownTime_ += Frame::DeltaTime();
		if (timeDownTime_ >= kTimeDownTime_) {
			Frame::SetTimeScale(1.0f);
		}
	}

	switch (order_) {
	case Order::FIRSTWAIT:

		///----------------------------------------------------
		/// 最初の硬直
		///----------------------------------------------------
		firstWaitTime_ += Frame::DeltaTime();
		if (firstWaitTime_ >= firstWaitTimeMax_) {
			stopCollisionBox_->IsAdapt(true);
			order_ = Order::RPUNCH;
		}
		break;

	case Order::RPUNCH:
		///----------------------------------------------------
		/// 右パンチ
		///----------------------------------------------------
		
		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailForthComboUpdate(pPlayer_->GetRightHand()->GetRailRunSpeedForth());

		stopCollisionBox_->SetPosition(pPlayer_->GetRightHand()->GetWorldPosition());
		stopCollisionBox_->Update();
		
		// イージング終了時の処理
		if (stopRailManager_->GetRailMoveTime() < 1.0f) break;

		stopRailManager_->SetRailMoveTime(1.0f);
		pPlayer_->GetRightHand()->RailForthComboUpdate(0.0f);

		stopCollisionBox_->IsAdapt(false);
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
		thrustCollisionBox_->IsAdapt(true);

		order_ = Order::LPUNCH;
		break;

	case Order::LPUNCH:
		///----------------------------------------------------
		/// 左パンチ
		///----------------------------------------------------

		// レール更新と座標反映
		pPlayer_->GetLeftHand()->RailForthComboUpdate(pPlayer_->GetLeftHand()->GetRailRunSpeedForth());
		thrustCollisionBox_->SetPosition(pPlayer_->GetLeftHand()->GetWorldPosition());
		thrustCollisionBox_->Update();

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

			/// コンボ先行予約
			BaseComboAattackBehavior::PreOderNextComboForButton();
			

		// イージング終了時の処理
		if (thrustRailManager_->GetRailMoveTime() > 0.0f) break;

		thrustRailManager_->SetRailMoveTime(0.0f);
		pPlayer_->GetLeftHand()->RailForthComboUpdate(0.0f);
		

		order_ = Order::WAIT;

		break;

	case Order::WAIT:
	
		waitTine_ += Frame::DeltaTimeRate();
		pPlayer_->Fall(fallInitSpeed_);

		if (pPlayer_->GetWorldPosition().y <= pPlayer_->InitY_) {
			
			if (waitTine_ >= pPlayer_->GetWaitTime(Player::FORTH)) {

				
				Frame::SetTimeScale(1.0f);

				pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
			}
		}
		else {
			/// ボタンで次のコンボ
			BaseComboAattackBehavior::PreOderNextComboForButton();
			BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<ComboAttackJumpFirst>(pPlayer_));
		}
	}

}

void ComboAttackForth::Debug() {

}