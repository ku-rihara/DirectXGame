/// behavior
#include"ComboAttackThird.h"
#include"ComboAttackRoot.h"

/// objs
#include"Player/Player.h"

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
	Vector3 collisionSize = Vector3::UnitVector();
	collisionBox_->SetSize(collisionSize);// 当たり判定サイズ

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

		upperJumpEaseT_ = std::min(upperJumpEaseT_, pPlayer_->GetPunchEaseMax(Player::THIRD));

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailUpdate(pPlayer_->GetRightHand()->GetRailRunSpeed());
		pPlayer_->GetRightHand()->SetWorldPosition(railManager_->GetPositionOnRail());

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

		/*/// 3コンボ目に移行
		else if (Input::GetInstance()->TrrigerKey(DIK_H)) {
			pPlayer_->ChangeComboBehavior
			(std::make_unique<ComboAttackSecond>(pPlayer_));
		}*/

		break;

	case Order::FALL:

		pPlayer_->Move(pPlayer_->GetMoveSpeed());/// 
		pPlayer_->Fall();

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailUpdate(-pPlayer_->GetRightHand()->GetRailRunSpeed());
		pPlayer_->GetRightHand()->SetWorldPosition(railManager_->GetPositionOnRail());

		if (pPlayer_->GetWorldPosition().y > pPlayer_->InitY_)break;
		if (railManager_->GetRailMoveTime() > 0.0f) break;

		pPlayer_->ChangeComboBehavior
		(std::make_unique<ComboAttackRoot>(pPlayer_));
		
		break;
	}

}

void ComboAttackThird::Debug() {

}