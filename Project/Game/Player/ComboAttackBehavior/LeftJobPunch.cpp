/// behavior
#include "LeftJobPunch.h"
#include "ComboAttackRoot.h"
#include "RoringUpper.h"
#include"GameCamera/GameCamera.h"

/// objs
#include "Player/Player.h"
/// frame
#include "Frame/Frame.h"

// 初期化
LeftJobPunch::LeftJobPunch(Player* player)
    : BaseComboAattackBehavior("LeftJobPunch", player) {

    Init();
}

LeftJobPunch::~LeftJobPunch() {
}

void LeftJobPunch::Init() {

    BaseComboAattackBehavior::Init();

    waitTine_ = 0.0f;

    /// collisionType
    pPlayer_->GetAttackController()->ChangeAttackType(PlayerAttackController::AttackType::NORMAL);

    /// パンチ座標セット
    lHandStartPos_  = pPlayer_->GetLeftHand()->GetObjTransform().translation_;
    lHandTargetPos_ = pPlayer_->GetLeftHand()->GetObjTransform().LookAt(Vector3::ToForward()) * pPlayerParameter_->GetNormalComboParm(SECOND).attackReach;

    // ease param
    EasingInit();

    // 　モーション
    BaseComboAattackBehavior::AnimationInit();

    pPlayer_->SoundPunch();
    // 振る舞い順序初期化
    order_ = Order::INIT;
}

// 更新
void LeftJobPunch::Update() {

    // 　モーション
    BaseComboAattackBehavior::RotateMotionUpdate(0, GetRotateValue(), true);
    BaseComboAattackBehavior::FloatAnimationUpdate();
    /// スケール変化
    BaseComboAattackBehavior::ScalingEaseUpdate();

    pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);

    switch (order_) {

        ///-----------------------------------------------------------------------------
        /// 初期化
        ///------------------------------------------------------------------------------
    case Order::INIT:
        pPlayer_->GetGameCamera()->PlayAnimation("LeftPunch");
        order_ = Order::PUNCH;
        break;

        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------
    case Order::PUNCH:

        /// パンチイージング更新
        punchEase_.Update(atkSpeed_);
        pPlayer_->GetLeftHand()->SetObjTranslate(punchPosition_);

        pPlayer_->GetAttackController()->SetPosition(pPlayer_->GetLeftHand()->GetObjTransform().GetWorldPos());

        break;

        ///----------------------------------------------------
        /// バックパンチ
        ///----------------------------------------------------
    case Order::BACKPUNCH:
       
        pPlayer_->AdaptRotate();

        /// バックパンチイージング更新
        backPunchEase_.Update(atkSpeed_);
        pPlayer_->GetLeftHand()->SetObjTranslate(punchPosition_);

        break;

    case Order::WAIT:
        waitTine_ += atkSpeed_;
        pPlayer_->AdaptRotate();

        /// コンボ途切れ
        if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(SECOND).waitTime) {

            pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
        } else {
            BaseComboAattackBehavior::PreOderNextComboForButton();
            if (isNextCombo_) {
                BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<RoringUpper>(pPlayer_));
            }
        }
        break;
    }
}

void LeftJobPunch::EasingInit() {
    punchEase_.Init("PlayerLeftPunch");
    punchEase_.SetAdaptValue(&punchPosition_);
    punchEase_.Reset();

    punchEase_.SetOnFinishCallback([this]() {
        order_ = Order::BACKPUNCH;
    });

    backPunchEase_.Init("PlayerLeftBackPunch");
    backPunchEase_.SetAdaptValue(&punchPosition_);
    backPunchEase_.Reset();

    backPunchEase_.SetOnFinishCallback([this]() {
        order_ = Order::WAIT;
    });

    // start end Value Set
    punchEase_.SetStartValue(lHandStartPos_);
    punchEase_.SetEndValue(lHandTargetPos_);

    backPunchEase_.SetStartValue(lHandTargetPos_);
    backPunchEase_.SetEndValue(lHandStartPos_);
}

void LeftJobPunch::CollisionBoxInit() {
}

void LeftJobPunch::Debug() {
}
