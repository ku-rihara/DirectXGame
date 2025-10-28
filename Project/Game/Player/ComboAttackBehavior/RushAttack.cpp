/// behavior
#include "RushAttack.h"
#include "ComboAttackRoot.h"

/// Player
#include "Player/Player.h"
#include"GameCamera/GameCamera.h"
#include "CollisionBox/PlayerAttackController.h"

#include "Frame/Frame.h"
#include "PostEffect/PostEffectRenderer.h"
#include "PostEffect/RadialBlur.h"
#include <imgui.h>

// 初期化
RushAttack::RushAttack(Player* player)
    : BaseComboAattackBehavior("RushAttack", player) {
    Init();
  
}

RushAttack::~RushAttack() {
}

void RushAttack::Init() {

    BaseComboAattackBehavior::Init();

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

    pPlayer_->RotateReset();

    rushBlurEase_.Init("RushBlur.json");
    rushBlurEase_.SetAdaptValue(&tempBlurParam_);

    rushBlurEase_.SetOnWaitEndCallback([this]() {
        PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::NONE);
    });

    step_ = STEP::START; // 突進
}

// 更新
void RushAttack::Update() {
    switch (step_) {

        ///---------------------------------------------------------
        /// 開始
        ///---------------------------------------------------------
    case STEP::START:
        //エフェクト初期化
        PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::RADIALBLUR);
        pPlayer_->GetEffects()->RushAttackRingEffectEmit();
        pPlayer_->GetGameCamera()->PlayAnimation("PlayerRush");

        //攻撃タイプ変更
        pPlayer_->GetAttackController()->ChangeAttackType(PlayerAttackController::AttackType::RUSH);

        step_ = STEP::RUSH;

        break;

        ///---------------------------------------------------------
        /// 突撃
        ///---------------------------------------------------------
    case STEP::RUSH:

        pPlayer_->GetEffects()->RushAttackEmit();

        rushBlurEase_.Update( atkSpeed_);
        PostEffectRenderer::GetInstance()->GetRadialBlur()->SetBlurWidth(tempBlurParam_);

        // RHand
        handRMoveEase_.Update( atkSpeed_);
        pPlayer_->GetRightHand()->SetWorldPosition(tempRHandPos_);

        // LHand
        handLMoveEase_.Update( atkSpeed_);
        pPlayer_->GetLeftHand()->SetWorldPosition(tempLHandPos_);

        // rush
        rushEase_.Update( atkSpeed_);
        pPlayer_->SetWorldPosition(tempRushPos_);

        /// 当たり判定座標
      
        pPlayer_->GetAttackController()->SetPosition(pPlayer_->GetWorldPosition());
        

        break;
        ///---------------------------------------------------------
        /// 待機
        ///---------------------------------------------------------
    case STEP::WAIT:

        PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::NONE);

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
   
}
void RushAttack::EasingInit() {
    handRMoveEase_.Init("RHandMove.json");
    handRMoveEase_.SetAdaptValue(&tempRHandPos_);
    handRMoveEase_.Reset();

    handRMoveEase_.SetStartValue(initRHandPos_);
   
    handRMoveEase_.SetOnFinishCallback([this]() {

    });

    handLMoveEase_.Init("LHandMove.json");
    handLMoveEase_.SetAdaptValue(&tempLHandPos_);
    handLMoveEase_.Reset();

    handLMoveEase_.SetStartValue(initLHandPos_);
  
    handLMoveEase_.SetOnFinishCallback([this]() {

    });

    rushEase_.Init("rushAttack.json");
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