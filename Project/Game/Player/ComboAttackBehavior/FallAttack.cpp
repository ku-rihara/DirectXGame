/// behavior
#include "FallAttack.h"
#include "ComboAttackRoot.h"
#include "Player/PlayerBehavior/PlayerMove.h"
#include "RushAttack.h"

/// Player
#include "GameCamera/GameCamera.h"
#include "Player/Player.h"

#include "CollisionBox/PlayerAttackController.h"
#include "Frame/Frame.h"
#include <imgui.h>

// 初期化
FallAttack::FallAttack(Player* player)
    : BaseComboAattackBehavior("FallAttack", player) {

    Init();
}

FallAttack::~FallAttack() {
}

void FallAttack::Init() {

    BaseComboAattackBehavior::Init();

    step_ = STEP::PREACTION; // 落ちる

    fallRotateY_ = 0.0f;

    boundSpeed_          = pPlayerParameter_->GetParamaters().boundJump.jumpSpeed * 60.0f; // 1.4
    gravity_             = pPlayerParameter_->GetParamaters().boundJump.gravity * 60.0f; // 8.8
    boundFallSpeedLimit_ = pPlayerParameter_->GetParamaters().boundJump.fallSpeedLimit * 60.0f; // 5.2
    rotateXSpeed_        = 11.0f;
    rotateYSpeed_        = 20.0f;

    initRotate_ = pPlayer_->GetTransform().rotation_;

    // ハンド初期化
    playerInitPosY_   = pPlayer_->GetWorldPosition().y;
    fallInitPosLHand_ = pPlayer_->GetLeftHand()->GetTransform().translation_.y;
    fallInitPosRHand_ = pPlayer_->GetRightHand()->GetTransform().translation_.y;

    tempPosY_ = pPlayer_->GetWorldPosition().y;

    EasingInit();

    pPlayer_->ChangeBehavior(std::make_unique<PlayerMove>(pPlayer_));
}

// 更新
void FallAttack::Update() {
    switch (step_) {

        ///---------------------------------------------------------
        /// 予備動作
        ///---------------------------------------------------------
    case STEP::PREACTION:

        preActionPosYEase_.Update(atkSpeed_);
        pPlayer_->SetWorldPositionY(tempPosY_ + preActionPosY_);
        break;
        ///---------------------------------------------------------
        /// 落ちる
        ///---------------------------------------------------------

    case STEP::FALL:

        pPlayer_->GetLeftHand()->SetWorldPositionY(0.2f);
        pPlayer_->GetRightHand()->SetWorldPositionY(0.2f);

        fallRotateY_ += Frame::DeltaTimeRate() * rotateYSpeed_;
        pPlayer_->SetRotationY(fallRotateY_);

        // イージング適応
        fallEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->SetWorldPositionY(tempWorldPosY_);

        break;

        ///---------------------------------------------------------
        /// 落ちる
        ///---------------------------------------------------------
    case STEP::FALLFINISH:

        // transformセット
        pPlayer_->SetRotation(initRotate_);
        pPlayer_->GetLeftHand()->SetWorldPositionY(fallInitPosLHand_);
        pPlayer_->GetRightHand()->SetWorldPositionY(fallInitPosRHand_);
        pPlayer_->SetWorldPositionY(pPlayerParameter_->GetParamaters().startPos_.y);

        // 演出
        pPlayer_->GetEffects()->FallEffectRenditionInit();
        pPlayer_->GetGameCamera()->PlayShake("FallAttackCamera");
        pPlayer_->FallSound();

        // 攻撃タイプの変更
        pPlayer_->GetAttackController()->ChangeAttackType(PlayerAttackController::AttackType::FALL);

        step_ = STEP::LANDING;
        break;
        ///---------------------------------------------------------
        /// 着地
        ///---------------------------------------------------------
    case STEP::LANDING:

        BaseComboAattackBehavior::PreOderNextComboForButton(); // 次のコンボに移行可能

        /// スケール変化
        landScaleEasing_.Update(atkSpeed_);
        pPlayer_->SetScale(tempLandScale_);
        pPlayer_->GetAttackController()->SetPosition(pPlayer_->GetWorldPosition());

        // 回転する
        landRotateX_ += Frame::DeltaTimeRate() * rotateXSpeed_;
        pPlayer_->SetRotationX(landRotateX_);

        // 反動ジャンプ
        pPlayer_->Jump(boundSpeed_, boundFallSpeedLimit_, gravity_);

        // 次の振る舞い
        if (pPlayer_->GetTransform().translation_.y > pPlayerParameter_->GetParamaters().startPos_.y) {
            break;
        }
        pPlayer_->SetRotation(initRotate_);
        step_ = STEP::WAIT;
        break;
        ///---------------------------------------------------------
        /// 待機
        ///---------------------------------------------------------
    case STEP::WAIT:

        pPlayer_->SetWorldPositionY(pPlayerParameter_->GetParamaters().startPos_.y);
        waitTime_ += Frame::DeltaTime();

        if (waitTime_ >= pPlayerParameter_->GetJumpComboParm(FIRST).waitTime) {
            step_ = STEP::RETURNROOT;
        } else {
            BaseComboAattackBehavior::PreOderNextComboForButton(); // 次のコンボに移行可能
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

void FallAttack::EasingInit() {

    landScaleEasing_.Init("PlayerLandScaling", "PlayerLandScaling.json");
    landScaleEasing_.SetAdaptValue(&tempLandScale_);
    landScaleEasing_.Reset();

    fallEase_.Init("PlayerFallAttack", "PlayerFallAttack.json");
    fallEase_.SetAdaptValue(&tempWorldPosY_);
    fallEase_.Reset();
    fallEase_.SetStartValue(playerInitPosY_);
    fallEase_.SetEndValue(pPlayerParameter_->GetParamaters().startPos_.y);

    fallEase_.SetOnFinishCallback([this]() {
        step_ = STEP::FALLFINISH;
    });

    preActionPosYEase_.Init("preActionPosY", "preActionPosY.json");
    preActionPosYEase_.SetAdaptValue(&preActionPosY_);
    preActionPosYEase_.Reset();

    preActionPosYEase_.SetOnFinishCallback([this]() {
        pPlayer_->GetGameCamera()->PlayAnimation("FallAttackPreAction");
        step_ = STEP::FALL;
    });
}

void FallAttack::CollisionInit() {
}

void FallAttack::Debug() {
    ImGui::Text("FallAttack");
}