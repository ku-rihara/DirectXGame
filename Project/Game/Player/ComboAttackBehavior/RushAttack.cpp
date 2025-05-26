/// behavior
#include "RushAttack.h"
#include "ComboAttackRoot.h"

/// Player
#include "Player/Player.h"

#include "Frame/Frame.h"
#include <imgui.h>

// 初期化
RushAttack::RushAttack(Player* player)
    : BaseComboAattackBehavior("RushAttack", player) {

    ///---------------------------------------------------------
    /// 変数初期化
    ///---------------------------------------------------------
    handMoveEasing_.maxTime = 0.2f;

    /// 初期化座標とターゲット座標
    initRHandPos_ = pPlayer_->GetRightHand()->GetTransform().translation_;
    initLHandPos_ = pPlayer_->GetLeftHand()->GetTransform().translation_;
    targetRPos_   = initRHandPos_ + (Vector3::ToForward() * 2.0f);
    targetLPos_   = initLHandPos_ + (Vector3::ToForward() * 2.0f);

    collisionBox_ = std::make_unique<AttackCollisionBox>();
    collisionBox_->Init();
    collisionBox_->attackType_ = AttackCollisionBox::AttackType::RUSH;
    collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
    collisionBox_->SetSize(Vector3(2.0f, 2.0f, 2.0f)); // 当たり判定サイズ
    collisionBox_->Update();
    collisionBox_->IsAdapt(false);

    pPlayer_->FaceToTarget();
    initPos_       = pPlayer_->GetWorldPosition();
    direction_     = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
    rushTargetPos_ = initPos_ + (direction_ * pPlayerParameter_->GetJumpComboParm(SECOND).attackReach);

    rushEaseTime_ = 0.0f;

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
        pPlayer_->GetEffects()->RushAttackRingEffectEmit();
        step_ = STEP::RUSH;

        break;

         ///---------------------------------------------------------
        /// 突撃
        ///---------------------------------------------------------
    case STEP::RUSH:
       
        pPlayer_->GetEffects()->RushAttackEmit();

        handMoveEasing_.time += Frame::DeltaTimeRate();
        rushEaseTime_ += Frame::DeltaTimeRate();

        /// ハンド
        handMoveEasing_.time = std::min(handMoveEasing_.time, handMoveEasing_.maxTime);

        pPlayer_->GetRightHand()->SetWorldPosition(
            EaseInExpo(initRHandPos_, targetRPos_, handMoveEasing_.time, handMoveEasing_.maxTime));

        pPlayer_->GetLeftHand()->SetWorldPosition(
            EaseInExpo(initLHandPos_, targetLPos_, handMoveEasing_.time, handMoveEasing_.maxTime));

        /// 突進
        pPlayer_->SetWorldPosition(
            EaseOutQuart(initPos_, rushTargetPos_, rushEaseTime_, pPlayerParameter_->GetJumpComboParm(SECOND).attackEaseMax));

        /// 当たり判定座標
        collisionBox_->IsAdapt(true);
        collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
        collisionBox_->Update();

        // 早期break
        if (rushEaseTime_ < pPlayerParameter_->GetJumpComboParm(SECOND).attackEaseMax) {
            break;
        };
        pPlayer_->GetRightHand()->SetWorldPosition(initRHandPos_);
        pPlayer_->GetLeftHand()->SetWorldPosition(initLHandPos_);
        step_ = STEP::WAIT;

        break;
        ///---------------------------------------------------------
        /// 待機
        ///---------------------------------------------------------
    case STEP::WAIT:
        
        collisionBox_->IsAdapt(false);
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

void RushAttack::Debug() {
    ImGui::Text("RushAttack");
}