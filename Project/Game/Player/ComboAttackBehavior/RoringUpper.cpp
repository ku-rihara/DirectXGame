/// behavior
#include "RoringUpper.h"
#include "ComboAttackRoot.h"
#include "JumpRush.h"
#include "ThrustPunch.h"
#include <numbers>
/// objs
#include "Player/Player.h"

// input
#include "JoyState/JoyState.h"

/// math
#include "Frame/Frame.h"

// 初期化
RoringUpper::RoringUpper(Player* player)
    : BaseComboAattackBehavior("RoringUpper", player) {

    ///---------------------------------------------------------
    /// 変数初期化
    ///---------------------------------------------------------

    initPos_          = pPlayer_->GetWorldPosition();
    forwardDirection_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());

    /// parm
    waitTine_            = 0.0f;
    collisionSize_       = 4.5f;
    kCollisionAliveTime_ = 0.3f;

    /// collisionBox
    CollisionInit();

    // rail
    RailInit();

    // backlash
    backlashPos_ = initPos_ + (forwardDirection_ * -pPlayerParameter_->GetParamaters().upperParm.BackLashValue);
    /*backlashEase_.maxTime = pPlayerParameter_->GetParamaters().upperParm.BackLashEaseTime;*/

    EasingInit();//イージング初期化

    jumpPower_      = pPlayerParameter_->GetParamaters().upperJump.jumpSpeed * 60.0f; // 1.4
    gravity_        = pPlayerParameter_->GetParamaters().upperJump.gravity * 60.0f; // 8.8
    fallSpeedLimit_ = pPlayerParameter_->GetParamaters().upperJump.fallSpeedLimit * 60.0f; // 5.2

    // 　モーション
    BaseComboAattackBehavior::AnimationInit();

    // hitstop
 /*   kHitStopTime_ = 0.1f;*/
    isHitStop_    = false;

    // 音
    pPlayer_->SoundPunch();

    // 振る舞い順序初期化
    order_         = Order::UPPER;
    fallInitSpeed_ = 0.0f;

    collisionAdaptTime_ = 0.0f;
}

RoringUpper::~RoringUpper() {
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
        /*backlashEase_.time += Frame::DeltaTimeRate();*/

        collisionBox_->SetPosition(pPlayer_->GetWorldPosition());

        // 0.3秒で当たり判定消す
        if (collisionAdaptTime_ >= kCollisionAliveTime_) {
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
        if (pPlayer_->GetTransform().translation_.y > pPlayerParameter_->GetParamaters().startPos_.y)
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
        if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(THIRD).waitTime) {

            order_ = Order::END;
        }

        else {
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
    collisionBox_->SetSize(Vector3::UnitVector() * collisionSize_); // 当たり判定サイズ
    collisionBox_->SetOffset(forwardDirection_ * 3.0f);
    collisionBox_->IsAdapt(false);
}

void RoringUpper::EasingInit() {
    backlashEase_.Init("UpperBackLash");
    backlashEase_.ApplyFromJson("UpperBackLash.json");
    backlashEase_.SaveAppliedJsonFileName();
    backlashEase_.SetAdaptValue(&tempWorldPos_);
    backlashEase_.Reset();

    backlashEase_.SetStartValue(initPos_);
    backlashEase_.SetEndValue(backlashPos_);

    rotateEase_.Init("upperRotate");
    rotateEase_.ApplyFromJson("upperRotate.json");
    rotateEase_.SaveAppliedJsonFileName();
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
    if (collisionBox_->GetIsHitStop() && !isHitStop_) {
        pPlayer_->GetEffects()->SpecialAttackRenditionInit();
        isHitStop_ = true;
    }
}

void RoringUpper::AnimationMove() {
    /// minを返す
    railManager_->SetRailMoveTime(std::min(railManager_->GetRailMoveTime(), 1.0f));
    pPlayer_->GetRightHand()->RailThreeComboUpdate(pPlayer_->GetRightHand()->GetRailRunSpeedThree());

    // バック
    backlashEase_.Update(Frame::DeltaTimeRate());
    /*  preWorldPos_ = EaseInCubic(initPos_, backlashPos_, backlashEase_.time, backlashEase_.maxTime);*/
    pPlayer_->SetWorldPositionX(tempWorldPos_.x);
    pPlayer_->SetWorldPositionZ(tempWorldPos_.z);

    // 回転
    rotateEase_.Update(Frame::DeltaTimeRate());
    /*xRotate_ = EaseOutCubic(0.0f, -std::numbers::pi_v<float>*2.0f, backlashEase_.time, backlashEase_.maxTime);*/
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