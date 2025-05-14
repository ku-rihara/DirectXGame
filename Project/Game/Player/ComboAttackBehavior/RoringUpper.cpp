/// behavior
#include"RoringUpper.h"
#include"JumpRush.h"
#include"ComboAttackRoot.h"
#include"ThrustPunch.h"
#include<numbers>
/// objs
#include"Player/Player.h"

// input
#include"JoyState/JoyState.h"

/// math
#include"Frame/Frame.h"

//初期化
RoringUpper::RoringUpper(Player* player)
	: BaseComboAattackBehavior("RoringUpper", player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	initPos_ = pPlayer_->GetWorldPosition();
	 forwardDirection_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());

	/// parm
	punchEase_.time = 0.0f;
	waitTine_ = 0.0f;
	upperJumpEaseT_ = 0.0f;
	collisionSize_ = 4.5f;
	kCollisionAliveTime_ = 0.3f;
	
	/// collisionBox
	CollisionInit();

	//rail
	RailInit();

	//backlash
	backlashPos_ = initPos_ + (forwardDirection_*-pPlayer_->GetParamater().upperParm.BackLashValue);
	backlashEase_.maxTime = pPlayer_->GetParamater().upperParm.BackLashEaseTime;

	jumpPower_ = pPlayer_->GetParamater().upperJump.jumpSpeed * 60.0f;//1.4
	gravity_ = pPlayer_->GetParamater().upperJump.gravity * 60.0f;//8.8
	fallSpeedLimit_ = pPlayer_->GetParamater().upperJump.fallSpeedLimit * 60.0f;//5.2

	//　モーション
	BaseComboAattackBehavior::AnimationInit();

	//hitstop
	kHitStopTime_ = 0.1f;
	isHitStop_ = false;

	// 音
	pPlayer_->SoundPunch();

	// 振る舞い順序初期化
	order_ = Order::UPPER;
	fallInitSpeed_ = 0.0f;

}

RoringUpper::~RoringUpper() {

}

//更新
void RoringUpper::Update() {

	BaseComboAattackBehavior::ScalingEaseUpdate();
	// 向き変更
	pPlayer_->Move(pPlayer_->GetParamater().moveSpeed);

	// ヒットストップ
	HitStopUpdate();

	switch (order_) {

		///----------------------------------------------------
		/// アッパー
		///----------------------------------------------------
	case Order::UPPER:

		// タイム加算
		upperJumpEaseT_ += Frame::DeltaTimeRate();
		backlashEase_.time += Frame::DeltaTimeRate();

		collisionBox_->SetPosition(pPlayer_->GetWorldPosition());

		///0.3秒で当たり判定消す
		if (backlashEase_.time >= kCollisionAliveTime_) {
			collisionBox_->IsAdapt(false);
			/// ボタンで次のコンボ
			BaseComboAattackBehavior::PreOderNextComboForButton();
		} else {
			collisionBox_->IsAdapt(true);
		}

		/// コリジョンボックス更新
		collisionBox_->Update();

		// プレイヤーのモーション
		AnimationMove();

		// 終了時の処理
        if (pPlayer_->GetTransform().translation_.y > pPlayer_->GetParamater().startPos_.y)
            break;
		
		Frame::SetTimeScale(1.0f);
        pPlayer_->PositionYReset();
		railManager_->SetRailMoveTime(0.0f);
		pPlayer_->GetRightHand()->RailThreeComboUpdate(0.0f);

		ChangeOrder();

		break;

		///----------------------------------------------------
		/// 待機
		///----------------------------------------------------
	case Order::WAIT:

		waitTine_ += Frame::DeltaTime();

		/// コンボ途切れ
		if (waitTine_ >= pPlayer_->GetNormalComboParm(Player::ComboNum::THIRD).waitTime) {
			
			order_ = Order::END;
		}

		else
		{
			/// ボタンで次のコンボ
			BaseComboAattackBehavior::PreOderNextComboForButton();
			if (isNextCombo_) {
				BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<JumpRush>(pPlayer_));
			}
		}

		break;

	case Order::END:

		pPlayer_->ChangeCombBoRoot();

		break;
	}

}

void RoringUpper::Debug() {

}

void RoringUpper::CollisionInit() {
	collisionBox_ = std::make_unique<AttackCollisionBox>();
	collisionBox_->Init();
	collisionBox_->attackType_ = AttackCollisionBox::AttackType::UPPER;
	collisionBox_->SetSize(Vector3::UnitVector() * collisionSize_);// 当たり判定サイズ
	collisionBox_->SetOffset(forwardDirection_ * 3.0f);
	collisionBox_->IsAdapt(false);
}

void RoringUpper::RailInit() {
	railManager_ = pPlayer_->GetRightHand()->GetThreeComboRailManager();
	railManager_->SetRailMoveTime(0.0f);
	railManager_->SetIsRoop(false);
}

void RoringUpper::HitStopUpdate() {
	//デルタタイムスケール小さく
	if (collisionBox_->GetIsHitStop() && !isHitStop_) {
		pPlayer_->StartEffectEmit();
		isHitStop_ = true;
	}

}

void RoringUpper::AnimationMove() {
	/// minを返す
	backlashEase_.time = std::min(backlashEase_.time, backlashEase_.maxTime);
	railManager_->SetRailMoveTime(std::min(railManager_->GetRailMoveTime(), 1.0f));

	// レール更新と座標反映
	pPlayer_->GetRightHand()->RailThreeComboUpdate(pPlayer_->GetRightHand()->GetRailRunSpeedThree());

	//バック
	preWorldPos_ = EaseInCubic(initPos_, backlashPos_, backlashEase_.time, backlashEase_.maxTime);
	pPlayer_->SetWorldPositionX(preWorldPos_.x);
	pPlayer_->SetWorldPositionZ(preWorldPos_.z);

	//回転
	xRotate_ = EaseOutCubic(0.0f, -std::numbers::pi_v<float>*2.0f, backlashEase_.time, backlashEase_.maxTime);
	pPlayer_->SetRotationX(xRotate_);

	pPlayer_->Jump(jumpPower_, fallSpeedLimit_, gravity_);
}

void RoringUpper::ChangeOrder() {
	if (isHitStop_) {
		order_ = Order::WAIT;
	} else {
		order_ = Order::END;
	}
}