/// behavior
#include"FallAttack.h"
#include"RushAttack.h"
#include"ComboAttackRoot.h"
#include"Player/PlayerBehavior/PlayerMove.h"

/// Player
#include"Player/Player.h"
#include"GameCamera/GameCamera.h"

#include<imgui.h>
#include"Frame/Frame.h"

//初期化
FallAttack::FallAttack(Player* player)
	: BaseComboAattackBehavior("FallAttack",player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	step_ = STEP::FALL; // 落ちる

	fallRotateY_ = 0.0f;

	boundSpeed_ = pPlayerParameter_->GetParamaters().bountJump.jumpSpeed*60.0f;//1.4
	gravity_ = pPlayerParameter_->GetParamaters().bountJump.gravity * 60.0f;//8.8
	boundFallSpeedLimit_ = pPlayerParameter_->GetParamaters().bountJump.fallSpeedLimit * 60.0f;//5.2
	rotateXSpeed_ = 11.0f;
	rotateYSpeed_ = 20.0f;

	/// collisionBox
	collisionBox_ = std::make_unique<AttackCollisionBox>();
	collisionBox_->Init();
	collisionBox_->attackType_ = AttackCollisionBox::AttackType::FALL;
	collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
	collisionBox_->SetSize(Vector3(4.5f,2.0f,4.5f));// 当たり判定サイズ
	collisionBox_->Update();
	collisionBox_->IsAdapt(false);

	initRotate_ = pPlayer_->GetTransform().rotation_;

	///land
	landScaleEasing_.maxTime = 0.5f;
	landScaleEasing_.amplitude = 0.6f;
	landScaleEasing_.period = 0.2f;

	// ハンド初期化
	playerInitPosY_ = pPlayer_->GetWorldPosition().y;
	fallInitPosLHand_= pPlayer_->GetLeftHand()->GetTransform().translation_.y;
	fallInitPosRHand_= pPlayer_->GetRightHand()->GetTransform().translation_.y;

	pPlayer_->ChangeBehavior(std::make_unique<PlayerMove>(pPlayer_));
}

FallAttack::~FallAttack() {

}

//更新
void FallAttack::Update() {
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
            EaseInSine(playerInitPosY_, pPlayerParameter_->GetParamaters().startPos_.y, fallEaseT_, pPlayerParameter_->GetJumpComboParm(FIRST).attackEaseMax)
			);

		/// 着地の瞬間
		if (fallEaseT_ < pPlayerParameter_->GetJumpComboParm(FIRST).attackEaseMax)break;

		pPlayer_->SetRotation(initRotate_);
		pPlayer_->GetLeftHand()->SetWorldPositionY(fallInitPosLHand_);
		pPlayer_->GetRightHand()->SetWorldPositionY(fallInitPosRHand_);
		pPlayer_->SetWorldPositionY(pPlayerParameter_->GetParamaters().startPos_.y);

		pPlayer_->GetEffects()->FallEffectRenditionInit(pPlayer_->GetWorldPosition());

		pPlayer_->GetGameCamera()->ChangeShakeMode();
		pPlayer_->FallSound();
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
			collisionBox_->IsAdapt(true);
			collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
			collisionBox_->Update();
		}
		else {
			collisionBox_->IsAdapt(false);
		}

		// 回転する
		landRotateX_ += Frame::DeltaTimeRate() * rotateXSpeed_;
		pPlayer_->SetRotationX(landRotateX_);

		pPlayer_->Jump(boundSpeed_, boundFallSpeedLimit_, gravity_);

	// 次の振る舞い
		if (pPlayer_->GetTransform().translation_.y > pPlayerParameter_->GetParamaters().startPos_.y) break;
		pPlayer_->SetRotation(initRotate_);
		step_ = STEP::WAIT;
		break;
		///---------------------------------------------------------
		/// 待機
		///---------------------------------------------------------
	case STEP::WAIT:
		
		pPlayer_->SetWorldPositionY(pPlayerParameter_->GetParamaters().startPos_.y);
		collisionBox_->IsAdapt(false);
		waitTime_ += Frame::DeltaTime();
		
		if (waitTime_ >= pPlayerParameter_->GetJumpComboParm(FIRST).waitTime) {
			step_ = STEP::RETURNROOT;
		}
		else {
			BaseComboAattackBehavior::PreOderNextComboForButton();//次のコンボに移行可能
			BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<RushAttack>(pPlayer_));
		}
		break;
	case STEP::RETURNROOT:
		pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
		break;
	default:
		break;
	}
	
}


void  FallAttack::Debug() {
	ImGui::Text("FallAttack");
}