/// behavior
#include "ThrustPunch.h"
#include "ComboAttackRoot.h"
#include "FallAttack.h"
#include "RightJobPunch.h"

/// objs
#include "GameCamera/GameCamera.h"
#include "Player/Player.h"

// offscreen
#include "PostEffect/PostEffectRenderer.h"

/// frame
#include "Frame/Frame.h"

// 初期化
ThrustPunch::ThrustPunch(Player* player)
    : BaseComboAattackBehavior("ThrustPunch", player) {

    ///---------------------------------------------------------
    /// 変数初期化
    ///---------------------------------------------------------

    firstWaitTimeMax_ = 0.2f;
    pPlayer_->SetHeadRotateX(0.0f);

    stopRailManager_ = pPlayer_->GetRightHand()->GetStopRailManager();
    stopRailManager_->SetIsRoop(false);

    thrustRailManager_ = pPlayer_->GetLeftHand()->GetThrustRailManager();
    thrustRailManager_->SetIsRoop(false);

    BaseComboAattackBehavior::AnimationInit();
  
    kTimeDownTime_ = 0.65f;
    timeDownTime_  = 0.0f;
    istimeSlow_    = false;

    order_         = Order::START; // 振る舞い順序初期化
    fallInitSpeed_ = 0.0f;

    stopRailManager_->SetRailMoveTime(0.0f);
    pPlayer_->GetRightHand()->RailForthComboUpdate(0.0f);

    ////音
    // pPlayer_->SoundPunch();
}

ThrustPunch::~ThrustPunch() {
}

// 更新
void ThrustPunch::Update() {
    BaseComboAattackBehavior::RotateMotionUpdate(GetRotateValueAnti(), GetRotateValue(), false);

    ChangeSlow();

    switch (order_) {
        ///-----------------------------------------------------------------------------------------------------
        /// 攻撃開始処理
        ///-----------------------------------------------------------------------------------------------------
    case Order::START:
       
        pPlayer_->GetAttackController()->ChangeAttackType(PlayerAttackController::AttackType::THRUST);
        pPlayer_->GetAttackController()->SetPosition(pPlayer_->GetWorldPosition());

        order_ = Order::LPUNCH;

        break;
        ///-----------------------------------------------------------------------------------------------------
        /// 左パンチ
        ///-----------------------------------------------------------------------------------------------------
    case Order::LPUNCH:

        // レール更新と座標反映
        pPlayer_->GetLeftHand()->RailForthComboUpdate(pPlayer_->GetLeftHand()->GetRailRunSpeedForth());
       
        // イージング終了時の処理
        if (thrustRailManager_->GetRailMoveTime() < 1.0f)
            break;

        thrustRailManager_->SetRailMoveTime(1.0f);
        pPlayer_->GetLeftHand()->RailForthComboUpdate(0.0f);
        order_ = Order::LBACKPUNCH;

        break;

        ///-----------------------------------------------------------------------------------------------------
        /// 左パンチ(戻る)
        ///-----------------------------------------------------------------------------------------------------
    case Order::LBACKPUNCH:

        // レール更新と座標反映
        pPlayer_->GetLeftHand()->RailForthComboUpdate(-pPlayer_->GetLeftHand()->GetRailRunSpeedForth());

        /// コンボ先行予約
        BaseComboAattackBehavior::PreOderNextComboForButton();

        // イージング終了時の処理
        if (thrustRailManager_->GetRailMoveTime() > 0.0f)
            break;

        thrustRailManager_->SetRailMoveTime(0.0f);
        pPlayer_->GetLeftHand()->RailForthComboUpdate(0.0f);

        order_ = Order::WAIT;

        break;

        ///-----------------------------------------------------------------------------------------------------
        /// 待機
        ///-----------------------------------------------------------------------------------------------------
    case Order::WAIT:

        waitTine_ += Frame::DeltaTimeRate();
        pPlayer_->Fall(fallInitSpeed_, pPlayerParameter_->GetParamaters().normalJump.fallSpeedLimit, pPlayerParameter_->GetParamaters().normalJump.gravity);
        pPlayer_->SetHeadScale(Vector3::UnitVector());

        if (pPlayer_->GetWorldPosition().y <= pPlayerParameter_->GetParamaters().startPos_.y) {
            pPlayer_->PositionYReset();
            if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(FORTH).waitTime) {

                Frame::SetTimeScale(1.0f);
                pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
            }
        } else {
            /// ボタンで次のコンボ
            BaseComboAattackBehavior::PreOderNextComboForButton();
            BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<FallAttack>(pPlayer_));
        }
    }
}

void ThrustPunch::Debug() {
}

void ThrustPunch::ChangeSlow() {
    // デルタタイムスケール小さく
    if (pPlayer_->GetAttackController()->GetIsSlow() && !istimeSlow_) {
        Frame::SetTimeScale(0.001f);
        PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::GRAY);
        pPlayer_->SoundStrongPunch();
        pPlayer_->GetGameCamera()->ChangeZoomInOut();
        istimeSlow_ = true;
    }

    // スロータイム
    if (istimeSlow_) {
        timeDownTime_ += Frame::DeltaTime();
        if (timeDownTime_ >= kTimeDownTime_) {
            /// スケール変化
            BaseComboAattackBehavior::ScalingEaseUpdate();
            Frame::SetTimeScale(1.0f);
            PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::NONE);
        }
    }
}

void ThrustPunch::ThrustCollisionSet() {
    // collisionBox_->Init();
    // collisionBox_->SetSize(Vector3::UnitVector() * 5.5f); // 当たり判定サイズ
    // Vector3 tforwardDirection = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
    // collisionBox_->SetOffset(tforwardDirection * 3.0f);
    // collisionBox_->SetPosition(pPlayer_->GetWorldPosition());
    // collisionBox_->Update();
    // collisionBox_->SetIsAdapt(true);
    // collisionBox_->attackType_ = PlayerAttackController::AttackType::THRUST;
}