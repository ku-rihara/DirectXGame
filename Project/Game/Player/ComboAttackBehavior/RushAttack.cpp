/// behavior
#include "RushAttack.h"
#include "ComboAttackRoot.h"

/// Player
#include "Player/Player.h"

#include "Frame/Frame.h"
#include "PostEffect/PostEffectRenderer.h"
#include "PostEffect/RadialBlur.h"
#include <imgui.h>

// 初期化
RushAttack::RushAttack(Player* player)
    : BaseComboAattackBehavior("RushAttack", player) {

    ///---------------------------------------------------------
    /// 変数初期化
    ///---------------------------------------------------------
    /*  handMoveEasing_.maxTime = 0.2f;*/

    /// 初期化座標とターゲット座標
    initRHandPos_ = pPlayer_->GetRightHand()->GetTransform().translation_;
    initLHandPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
    targetRPos_   = initRHandPos_ + (Vector3::ToForward() * 2.0f);
    targetLPos_   = initLHandPos_ + (Vector3::ToForward() * 2.0f);

    CollisionInit();

    pPlayer_->FaceToTarget();
    initPos_       = pPlayer_->GetWorldPosition();
    direction_     = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
    rushTargetPos_ = initPos_ + (direction_ * pPlayerParameter_->GetJumpComboParm(SECOND).attackReach);

    EasingInit();

 
    rushBlurEase_.Init("RushEffect");
    rushBlurEase_.ApplyFromJson("RushBlur.json");
    rushBlurEase_.SaveAppliedJsonFileName();
    rushBlurEase_.SetAdaptValue(&tempBlurParam_);
    rushBlurEase_.Reset();

    rushBlurEase_.SetOnWaitEndCallback([this]() {
        PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::NONE);
    });

    step_ = STEP::EMIT; // 突進
}

RushAttack::~RushAttack() {
}

// 更新
void RushAttack::Update() {
    switch (step_) {

        ///---------------------------------------------------------
        /// エフェクト発射
        ///---------------------------------------------------------
    case STEP::EMIT:
        PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::RADIALBLUR);
        pPlayer_->GetEffects()->RushAttackRingEffectEmit();
        step_ = STEP::RUSH;

        break;

        ///---------------------------------------------------------
        /// 突撃
        ///---------------------------------------------------------
    case STEP::RUSH:

        pPlayer_->GetEffects()->RushAttackEmit();

        rushBlurEase_.Update(Frame::DeltaTimeRate());
        PostEffectRenderer::GetInstance()->GetRadialBlur()->SetBlurWidth(tempBlurParam_);

        // Rhand
        handRMoveEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->GetRightHand()->SetWorldPosition(tempRHandPos_);

        // Lhand
        handLMoveEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->GetLeftHand()->SetWorldPosition(tempLHandPos_);

        // rush
        rushEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->SetWorldPosition(tempRushPos_);

        /// 当たり判定座標
        collisionBox_->SetIsAdapt(true);
        collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
        collisionBox_->Update();

        break;
        ///---------------------------------------------------------
        /// 待機
        ///---------------------------------------------------------
    case STEP::WAIT:

        PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::NONE);

        collisionBox_->SetIsAdapt(false);
        waitTime_ += Frame::DeltaTime();
        if (waitTime_ < pPlayerParameter_->GetJumpComboParm(SECOND).waitTime) {
            break;
        }
        step_ = STEP::RETURNROOT;
        break;

    case STEP::RETURNROOT:
        pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
        break;

    default:
        break;
    }
}

void RushAttack::CollisionInit() {
    collisionBox_ = std::make_unique<PlayerAttackController>();
    collisionBox_->Init();
    collisionBox_->attackType_ = PlayerAttackController::AttackType::RUSH;
    collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
    collisionBox_->SetSize(Vector3(2.0f, 2.0f, 2.0f)); // 当たり判定サイズ
    collisionBox_->Update();
    collisionBox_->SetIsAdapt(false);
}
void RushAttack::EasingInit() {
    handRMoveEase_.Init("RHandMove");
    handRMoveEase_.SetAdaptValue(&tempRHandPos_);
    handRMoveEase_.Reset();

    handRMoveEase_.SetStartValue(initRHandPos_);
    /*  handRMoveEase_.SetEndValue(targetRPos_);*/

    handRMoveEase_.SetOnFinishCallback([this]() {

    });

    handLMoveEase_.Init("LHandMove");
    handLMoveEase_.SetAdaptValue(&tempLHandPos_);
    handLMoveEase_.Reset();

    handLMoveEase_.SetStartValue(initLHandPos_);
    /*  handLMoveEase_.SetEndValue(targetLPos_);*/

    handLMoveEase_.SetOnFinishCallback([this]() {

    });

    rushEase_.Init("rushAttack");
    rushEase_.SetAdaptValue(&tempRushPos_);
    rushEase_.Reset();

    rushEase_.SetStartValue(initPos_);
    rushEase_.SetEndValue(rushTargetPos_);

    rushEase_.SetOnFinishCallback([this]() {
        pPlayer_->GetRightHand()->SetWorldPosition(initRHandPos_);
        pPlayer_->GetLeftHand()->SetWorldPosition(initLHandPos_);
        step_ = STEP::WAIT;
    });
}

void RushAttack::Debug() {
    ImGui::Text("RushAttack");
}