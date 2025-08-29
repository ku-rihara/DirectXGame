/// behavior
#include"TitleFirstFall.h"
#include"TitleRightPunch.h"

#include"Player/PlayerBehavior/PlayerMove.h"

/// Player
#include"Player/Player.h"
#include"GameCamera/GameCamera.h"

#include<imgui.h>
#include"Frame/Frame.h"

//初期化
TitleFirstFall::TitleFirstFall(Player* player)
	: BaseTitleBehavior("TitleFirstFall",player) {

	///---------------------------------------------------------
	/// 変数初期化
	///---------------------------------------------------------

	step_ = STEP::FALL; // 落ちる

	fallRotateY_ = 0.0f;
	
	boundSpeed_ = 1.4f;
	gravity_ = 8.8f;
	boundFallSpeedLimit_ = -5.2f;
	rotateXSpeed_ = 11.0f;
	rotateYSpeed_ = 20.0f;

	initRotate_ = pPlayer_->GetTransform().rotation_;

	
	EasingInit();

	// ハンド初期化
	
	fallInitPosLHand_= pPlayer_->GetLeftHand()->GetTransform().translation_.y;
	fallInitPosRHand_= pPlayer_->GetRightHand()->GetTransform().translation_.y;

}

TitleFirstFall::~TitleFirstFall() {

}

//更新
void TitleFirstFall::Update() {
	switch (step_)
	{
	case STEP::FALL:
	///---------------------------------------------------------
	///落ちる
	///---------------------------------------------------------
        if (Frame::DeltaTimeRate() > 2.0f) {
            break;
        }
        fallEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->SetWorldPositionY(tempPosY_);

		fallRotateY_ += Frame::DeltaTimeRate()* rotateYSpeed_;
	

		break;
	case STEP::LANDING:
	///---------------------------------------------------------
	/// 着地
	///---------------------------------------------------------
	   
		/// スケール変化
    landScaleEasing_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetScale(tempScale_);
		
		// 回転する
		landRotateX_ += Frame::DeltaTimeRate() * rotateXSpeed_;
		pPlayer_->SetRotationX(landRotateX_);

		// Yに加算
		pPlayer_->AddPosition(Vector3(0, boundSpeed_, 0));
		// 加速する
		boundSpeed_ = max(boundSpeed_ - (gravity_ * Frame::DeltaTimeRate()), boundFallSpeedLimit_);

	// 次の振る舞い
        if (pPlayer_->GetTransform().translation_.y > pPlayerParameter_->GetParamaters().startPos_.y) {
            break;
        }

		pPlayer_->SetRotation(initRotate_);
        pPlayer_->SetWorldPositionY(pPlayerParameter_->GetParamaters().startPos_.y);
		step_ = STEP::WAIT;
		break;
	case STEP::WAIT:
		///---------------------------------------------------------
		/// 待機
		///---------------------------------------------------------
	
		waitTime_ += Frame::DeltaTime();
		
		if (waitTime_ >= pPlayerParameter_->GetJumpComboParm(FIRST).waitTime) {
			step_ = STEP::RETURNROOT;
		}
		break;
	case STEP::RETURNROOT:
		pPlayer_->ChangeTitleBehavior(std::make_unique<TitleRightPunch>(pPlayer_));
		break;
	default:
		break;
	}
	
}

void TitleFirstFall::EasingInit() {
    landScaleEasing_.Init("TitleLandScaleing");
    landScaleEasing_.ApplyFromJson("TitleLandScaleing.json");
    landScaleEasing_.SaveAppliedJsonFileName();
    landScaleEasing_.SetAdaptValue(&tempScale_);
    landScaleEasing_.Reset();

    landScaleEasing_.SetOnFinishCallback([this]() {
       
    });

	fallEase_.Init("TitleFall");
    fallEase_.ApplyFromJson("TitleFall.json");
    fallEase_.SaveAppliedJsonFileName();
    fallEase_.SetAdaptValue(&tempPosY_);
    fallEase_.Reset();

    fallEase_.SetOnFinishCallback([this]() {
        pPlayer_->SetRotation(initRotate_);
        pPlayer_->GetLeftHand()->SetWorldPositionY(fallInitPosLHand_);
        pPlayer_->GetRightHand()->SetWorldPositionY(fallInitPosRHand_);
        pPlayer_->SetWorldPositionY(pPlayerParameter_->GetParamaters().startPos_.y);
        pPlayer_->GetEffects()->FallEffectRenditionInit();
        step_ = STEP::LANDING;
    });
  }


void  TitleFirstFall::Debug() {
	ImGui::Text("TitleFirstFall");
}