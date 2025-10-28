/// behavior
#include "RoringUpper.h"
#include "ComboAttackRoot.h"
#include "JumpRush.h"
#include "ThrustPunch.h"
#include <numbers>
/// objs
#include "Player/Player.h"
#include"GameCamera/GameCamera.h"
#include"AttackEffect/AttackEffect.h"

// input
#include "input/Input.h"

/// math
#include "CollisionBox/PlayerAttackController.h"
#include "Frame/Frame.h"

// 初期化
RoringUpper::RoringUpper(Player* player)
    : BaseComboAattackBehavior("RoringUpper", player) {
    Init();
}

RoringUpper::~RoringUpper() {
}

void RoringUpper::Init() {

    BaseComboAattackBehavior::Init();
   
    initPos_          = pPlayer_->GetWorldPosition();
    forwardDirection_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());

    /// parm
    waitTine_            = 0.0f;
    kCollisionAliveTime_ = 0.3f;

    /// collisionBox
    pPlayer_->GetAttackController()->ChangeAttackType(PlayerAttackController::AttackType::UPPER);

    // rail
    RailInit();

    // backlash
    backlashPos_ = initPos_ + (forwardDirection_ * -pPlayerParameter_->GetParamaters().upperParam.BackLashValue);

    EasingInit(); // イージング初期化

    jumpPower_      = pPlayerParameter_->GetParamaters().upperJump.jumpSpeed * 60.0f; // 1.4
    gravity_        = pPlayerParameter_->GetParamaters().upperJump.gravity * 60.0f; // 8.8
    fallSpeedLimit_ = pPlayerParameter_->GetParamaters().upperJump.fallSpeedLimit * 60.0f; // 5.2

    // 　モーション
    BaseComboAattackBehavior::AnimationInit();

    // hitStop
    isHitStop_ = false;

    // 音
    pPlayer_->SoundPunch();

    // 振る舞い順序初期化
    order_         = Order::UPPER;
    fallInitSpeed_ = 0.0f;

    collisionAdaptTime_ = 0.0f;

    pPlayer_->GetAttackController()->ResetComboEffect();
}

// 更新
void RoringUpper::Update() {

    BaseComboAattackBehavior::ScalingEaseUpdate();
    // 向き変更
    pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);

    // ヒットストップ
    HitStopUpdate();

    switch (order_) {

        ///----------------------------------------------------
        /// アッパー
        ///----------------------------------------------------
    case Order::UPPER:

        // タイム加算
        collisionAdaptTime_ += Frame::DeltaTimeRate();
       
        pPlayer_->GetAttackController()->SetPosition(pPlayer_->GetWorldPosition());

       
        if (collisionAdaptTime_ >= kCollisionAliveTime_) {
            // ボタンで次のコンボ
            BaseComboAattackBehavior::PreOderNextComboForButton();
        }

        // プレイヤーのモーション
        AnimationMove();

        // 終了時の処理
        if (pPlayer_->GetTransform().translation_.y > pPlayerParameter_->GetParamaters().startPos_.y) {
            break;
        }
        pPlayer_->PositionYReset();
        railManager_->SetRailMoveTime(0.0f);
        pPlayer_->GetRightHand()->RailThreeComboUpdate(0.0f);

        ChangeOrder();
  
        break;

        ///----------------------------------------------------
        /// 待機
        ///----------------------------------------------------
    case Order::WAIT:

        waitTine_ += Frame::DeltaTimeRate();

        // コンボ途切れ
        if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(THIRD).waitTime) {
            order_ = Order::END;
        }

        else {
            // ボタンで次のコンボ
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

}

void RoringUpper::EasingInit() {
    backlashEase_.Init("UpperBackLash.json");
    backlashEase_.SetAdaptValue(&tempWorldPos_);
    backlashEase_.Reset();

    backlashEase_.SetStartValue(initPos_);
    backlashEase_.SetEndValue(backlashPos_);

    rotateEase_.Init("upperRotate.json");
    rotateEase_.SetAdaptValue(&xRotate_);
    rotateEase_.Reset();
}

void RoringUpper::RailInit() {
    railManager_ = pPlayer_->GetRightHand()->GetThreeComboRailManager();
    railManager_->SetRailMoveTime(0.0f);
    railManager_->SetIsRoop(false);
}

void RoringUpper::HitStopUpdate() {
    // デルタタイムスケール小さく
    if (pPlayer_->GetAttackController()->GetIsHitStop() && !isHitStop_) {
        pPlayer_->GetEffects()->SpecialAttackRenditionInit();
        pPlayer_->GetGameCamera()->PlayAnimation("UpperAtk");
        
        pPlayer_->GetAttackEffect()->PlayHitStop("UpperHitStop");
        isHitStop_ = true;
    }
}

void RoringUpper::AnimationMove() {
    /// minを返す
    railManager_->SetRailMoveTime(std::min(railManager_->GetRailMoveTime(), 1.0f));
    pPlayer_->GetRightHand()->RailThreeComboUpdate(pPlayer_->GetAttackController()->GetAttackSpeed(pPlayer_->GetRightHand()->GetRailRunSpeedThree()));
  
    // バック
    backlashEase_.Update(atkSpeed_);
    pPlayer_->SetWorldPositionX(tempWorldPos_.x);
    pPlayer_->SetWorldPositionZ(tempWorldPos_.z);

    // 回転
    rotateEase_.Update(atkSpeed_);
    pPlayer_->SetHeadRotateX(xRotate_);

    pPlayer_->Jump(jumpPower_, fallSpeedLimit_, gravity_);
}

void RoringUpper::ChangeOrder() {
    if (isHitStop_) {
        order_ = Order::WAIT;
    } else {
        order_ = Order::END;
    }
}