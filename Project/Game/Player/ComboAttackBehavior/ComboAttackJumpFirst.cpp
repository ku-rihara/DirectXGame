/// behavior
#include"ComboAttackJumpFirst.h"
#include"ComboAttackJumpSecond.h"
#include"ComboAttackRoot.h"
#include"Player/PlayerBehavior/PlayerRoot.h"

/// Player
#include"Player/Player.h"
#include"GameCamera/GameCamera.h"

#include<imgui.h>
#include"Frame/Frame.h"

//初期化
ComboAttackJumpFirst::ComboAttackJumpFirst(Player* player)
	: BaseComboAattackBehavior("ComboAttackJumpFirst",player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	step_ = STEP::FALL; // 落ちる

	fallCollisionBox_ = std::make_unique<FallCollisionBox>();
	fallCollisionBox_->Init();
	fallRotateY_ = 0.0f;

	boundSpeed_ = 1.4f;
	gravity_ = 8.8f;
	boundFallSpeedLimit_ = -5.2f;
	rotateXSpeed_ = 11.0f;
	rotateYSpeed_ = 20.0f;

	/// collisionBox
	fallCollisionBox_ = std::make_unique<FallCollisionBox>();
	fallCollisionBox_->Init();
	fallCollisionBox_->SetPosition(pPlayer_->GetWorldPosition());
	fallCollisionBox_->SetSize(Vector3(4.5f,2.0f,4.5f));// 当たり判定サイズ
	fallCollisionBox_->Update();
	fallCollisionBox_->IsAdapt(false);

	initRotate_ = pPlayer_->GetTransform().rotation_;

	///land
	landScaleEasing_.maxTime = 0.5f;
	landScaleEasing_.amplitude = 0.6f;
	landScaleEasing_.period = 0.2f;

	// ハンド初期化
	playerInitPosY_ = pPlayer_->GetWorldPosition().y;
	fallInitPosLHand_= pPlayer_->GetLeftHand()->GetTransform().translation_.y;
	fallInitPosRHand_= pPlayer_->GetRightHand()->GetTransform().translation_.y;

	pPlayer_->ChangeBehavior(std::make_unique<PlayerRoot>(pPlayer_));
}

ComboAttackJumpFirst::~ComboAttackJumpFirst() {

}

//更新
void ComboAttackJumpFirst::Update() {
	switch (step_)
	{
	case STEP::FALL:
	///---------------------------------------------------------
	///落ちる
	///---------------------------------------------------------

		fallEaseT_ += Frame::DeltaTimeRate();
		fallRotateY_ += Frame::DeltaTimeRate()* rotateYSpeed_;
	
		pPlayer_->GetLeftHand()->SetWorldPositionY(0.2f);
		pPlayer_->GetRightHand()->SetWorldPositionY(0.2f);
		pPlayer_->SetRotationY(fallRotateY_);

		/// プレイヤーが落ちる
		pPlayer_->SetWorldPositionY(
			EaseInSine(playerInitPosY_, Player::InitY_, fallEaseT_, pPlayer_->GetJPunchEaseMax(Player::JFIRST))
			);

		/// 着地の瞬間
		if (fallEaseT_ < pPlayer_->GetJPunchEaseMax(Player::JFIRST))break;

		pPlayer_->SetRotation(initRotate_);
		pPlayer_->GetLeftHand()->SetWorldPositionY(fallInitPosLHand_);
		pPlayer_->GetRightHand()->SetWorldPositionY(fallInitPosRHand_);
		pPlayer_->SetWorldPositionY(Player::InitY_);
		pPlayer_->FallParticleEmit();
		pPlayer_->FallEffectInit(pPlayer_->GetWorldPosition());
		pPlayer_->GetGameCamera()->ChangeShakeMode();
		step_ = STEP::LANDING;

		break;
	case STEP::LANDING:
	///---------------------------------------------------------
	/// 着地
	///---------------------------------------------------------
	   
		BaseComboAattackBehavior::PreOderNextComboForButton();//次のコンボに移行可能

		/// スケール変化
		landScaleEasing_.time += Frame::DeltaTimeRate();
		landScaleEasing_.time = std::min(landScaleEasing_.time, landScaleEasing_.maxTime);
		pPlayer_->SetScale(EaseAmplitudeScale(Vector3::UnitVector(), landScaleEasing_.time, landScaleEasing_.maxTime,
			                                  landScaleEasing_.amplitude, landScaleEasing_.period));

		if (landScaleEasing_.time <= 0.15f) {
			/// 当たり判定座標
			fallCollisionBox_->IsAdapt(true);
			fallCollisionBox_->SetPosition(pPlayer_->GetWorldPosition());
			fallCollisionBox_->Update();
		}
		else {
			fallCollisionBox_->IsAdapt(false);
		}

		// 回転する
		landRotateX_ += Frame::DeltaTimeRate() * rotateXSpeed_;
		pPlayer_->SetRotationX(landRotateX_);

		// Yに加算
		pPlayer_->AddPosition(Vector3(0, boundSpeed_, 0));
		// 加速する
		boundSpeed_ = max(boundSpeed_ - (gravity_ * Frame::DeltaTimeRate()), boundFallSpeedLimit_);

	// 次の振る舞い
		if (pPlayer_->GetTransform().translation_.y > pPlayer_->InitY_) break;
		pPlayer_->SetRotation(initRotate_);
		pPlayer_->SetWorldPositionY(Player::InitY_);
		step_ = STEP::WAIT;
		break;
	case STEP::WAIT:
		///---------------------------------------------------------
		/// 待機
		///---------------------------------------------------------
		fallCollisionBox_->IsAdapt(false);
		waitTime_ += Frame::DeltaTime();
		
		if (waitTime_ >= pPlayer_->GetJWaitTime(Player::JFIRST)) {
			step_ = STEP::RETURNROOT;
		}
		else {
			BaseComboAattackBehavior::PreOderNextComboForButton();//次のコンボに移行可能
			BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<ComboAttackJumpSecond>(pPlayer_));
		}
		break;
	case STEP::RETURNROOT:
		pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
		break;
	default:
		break;
	}
	
}


void  ComboAttackJumpFirst::Debug() {
	ImGui::Text("ComboAttackJumpFirst");
}