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
	/// trust
	thrustCollisionBox_->Init();
	
	order_ = Order::RPUNCH; // 振る舞い順序初期化
}

ComboAttackForth::~ComboAttackForth() {

}

//更新
void ComboAttackForth::Update() {

	thrustCollisionBox_->Update();

	switch (order_) {

	case Order::RPUNCH:
		///----------------------------------------------------
		/// パンチ
		///----------------------------------------------------


		break;

	case Order::BACKPUNCH:
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