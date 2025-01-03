/// behavior
#include"ComboAttackThird.h"
#include"ComboAttackRoot.h"
#include"ComboAttackForth.h"

/// objs
#include"Player/Player.h"

// input
#include"JoyState/JoyState.h"

/// math
#include"Frame/Frame.h"

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
	upperJumpEaseT_ = 0.0f;
	
	/// collisionBox
	collisionBox_ = std::make_unique<UpperCollisionBox>();
	collisionBox_->Init();

	collisionBox_->SetSize(Vector3::UnitVector());// 当たり判定サイズ
	collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
	Vector3 forwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	collisionBox_->SetOffset(forwardDirection * 4.0f);
	collisionBox_->IsAdapt(true);

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

	collisionBox_->Update();/// コリジョンボックス更新

	switch (order_) {

	case Order::UPPER:

		///----------------------------------------------------
		/// アッパー
		///----------------------------------------------------

		upperJumpEaseT_ += Frame::DeltaTimeRate();

		///0.3秒で当たり判定消す
		if (upperJumpEaseT_ >= kCollisionAliveTime_) {
			collisionBox_->IsAdapt(false);
		}

		upperJumpEaseT_ = std::min(upperJumpEaseT_, pPlayer_->GetPunchEaseMax(Player::THIRD));

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailUpdate(pPlayer_->GetRightHand()->GetRailRunSpeed());
		

		pPlayer_->SetWorldPositionY(
			EaseInSine(initPosY_,initPosY_+pPlayer_->GetUpperPosY(),upperJumpEaseT_, pPlayer_->GetPunchEaseMax(Player::THIRD))
		);

		// イージング終了時の処理
		if (railManager_->GetRailMoveTime() < 1.0f) break;

			railManager_->SetRailMoveTime(1.0f);
			order_ = Order::WAIT;

		break;

	case Order::WAIT:
		///----------------------------------------------------
		/// 待機
		///----------------------------------------------------
		
		waitTine_ += Frame::DeltaTime();

		/// コンボ途切れ
		if (waitTine_ >= pPlayer_->GetWaitTime(Player::THIRD)) {
			order_ = Order::FALL;
		}

		else {
			railManager_->SetRailMoveTime(0.0f);

			// レール更新と座標反映
			pPlayer_->GetRightHand()->RailUpdate(0.0f);
			/// ボタンで次のコンボ
			BaseComboAattackBehavior::ChangeNextComboForButton(std::make_unique<ComboAttackForth>(pPlayer_));
			break;
		}

		break;

	case Order::FALL:

		pPlayer_->Move(pPlayer_->GetMoveSpeed());/// 
		pPlayer_->Fall();

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailUpdate(-pPlayer_->GetRightHand()->GetRailRunSpeed());
		
		/// 3コンボ目終了の条件
		if (pPlayer_->GetWorldPosition().y > pPlayer_->InitY_)break;
		if (railManager_->GetRailMoveTime() > 0.0f) break;

		pPlayer_->SetWorldPositionY(pPlayer_->InitY_);
		railManager_->SetRailMoveTime(0.0f);

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailUpdate(0.0f);


		pPlayer_->ChangeComboBehavior
		(std::make_unique<ComboAttackRoot>(pPlayer_));
		
		break;
	}

}

void ComboAttackThird::Debug() {

}