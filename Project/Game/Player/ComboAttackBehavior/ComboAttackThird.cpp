/// behavior
#include"ComboAttackThird.h"
#include"ComboAttackRoot.h"
#include"ComboAttackForth.h"
#include<numbers>
/// objs
#include"Player/Player.h"

// input
#include"JoyState/JoyState.h"

/// math
#include"Frame/Frame.h"

//初期化
ComboAttackThird::ComboAttackThird(Player* player)
	: BaseComboAattackBehavior("ComboAttackThird", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	initPosY_ = pPlayer_->GetWorldPosition().y;

	/// parm
	punchEase_.time = 0.0f;
	waitTine_ = 0.0f;
	upperJumpEaseT_ = 0.0f;
	
	/// collisionBox
	collisionBox_ = std::make_unique<AttackCollisionBox>();
	collisionBox_->Init();
	collisionBox_->attackType_ = AttackCollisionBox::AttackType::UPPER;

	collisionBox_->SetSize(Vector3::UnitVector() * 4.5f);// 当たり判定サイズ
	
	Vector3 forwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
	collisionBox_->SetOffset(forwardDirection * 3.0f);
	collisionBox_->IsAdapt(false);

	railManager_ = pPlayer_->GetRightHand()->GetThreeComboRailManager();
	railManager_->SetRailMoveTime(0.0f);
	railManager_->SetIsRoop(false);

	rotateEase_.time = 0.0f;
	rotateEase_.maxTime = 0.7f;
	
	//　モーション
	BaseComboAattackBehavior::AnimationInit();

	// 音
	pPlayer_->SoundPunch();

	// 振る舞い順序初期化
	order_ = Order::UPPER;

	fallInitSpeed_ = 0.0f;

}

ComboAttackThird::~ComboAttackThird() {

}

//更新
void ComboAttackThird::Update() {

	BaseComboAattackBehavior::ScalingEaseUpdate();
	// 向き変更
	pPlayer_->Move(pPlayer_->GetPlayerParams().moveSpeed);

	
	pPlayer_->SetHeadRotateX(zRotate_);

	if (rotateEase_.time >= rotateEase_.maxTime) {
		rotateEase_.time = rotateEase_.maxTime;
		zRotate_ = 0.0f;
		pPlayer_->SetHeadRotateX(0.0f);
	}
	else {
		rotateEase_.time += Frame::DeltaTimeRate();
		zRotate_ += 70.0f * Frame::DeltaTimeRate();
	}
	switch (order_) {

	case Order::UPPER:
		
		///----------------------------------------------------
		/// アッパー
		///----------------------------------------------------

		upperJumpEaseT_ += Frame::DeltaTimeRate();
		collisionBox_->SetPosition(pPlayer_->GetWorldPosition());

		///0.3秒で当たり判定消す
		if (upperJumpEaseT_ >= kCollisionAliveTime_) {
			collisionBox_->IsAdapt(false);
			/// ボタンで次のコンボ
			BaseComboAattackBehavior::PreOderNextComboForButton();
		}
		else {
			collisionBox_->IsAdapt(true);
		}

		upperJumpEaseT_ = std::min(upperJumpEaseT_, pPlayer_->GetNormalComboParm(Player::ComboNum::THIRD).attackEaseMax);

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailThreeComboUpdate(pPlayer_->GetRightHand()->GetRailRunSpeedThree());
		
		pPlayer_->SetWorldPositionY(
			EaseInSine(initPosY_,initPosY_+pPlayer_->GetPlayerParams().upperPosY,upperJumpEaseT_, pPlayer_->GetNormalComboParm(Player::ComboNum::THIRD).attackEaseMax)
		);

		
		collisionBox_->Update();/// コリジョンボックス更新

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
		if (waitTine_ >= pPlayer_->GetNormalComboParm(Player::ComboNum::THIRD).waitTime) {
			order_ = Order::FALL;
		}

		else 
		{
			/// ボタンで次のコンボ
			BaseComboAattackBehavior::PreOderNextComboForButton();
			if (isNextCombo_) {
				BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<ComboAttackForth>(pPlayer_));
			}
		}

		break;

	case Order::FALL:

		pPlayer_->Move(pPlayer_->GetPlayerParams().moveSpeed);/// 
		pPlayer_->Fall(fallInitSpeed_);

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailThreeComboUpdate(-pPlayer_->GetRightHand()->GetRailRunSpeedThree());
		
		/// 3コンボ目終了の条件
		if (pPlayer_->GetWorldPosition().y > pPlayer_->InitY_)break;
		if (railManager_->GetRailMoveTime() > 0.0f) break;

		pPlayer_->SetWorldPositionY(pPlayer_->InitY_);
		railManager_->SetRailMoveTime(0.0f);

		// レール更新と座標反映
		pPlayer_->GetRightHand()->RailThreeComboUpdate(0.0f);

		pPlayer_->SetHeadRotateX(0.0f);
		pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
		
		break;
	}

}

void ComboAttackThird::Debug() {

}