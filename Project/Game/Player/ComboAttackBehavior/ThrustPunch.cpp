/// behavior
#include "ThrustPunch.h"
#include "ComboAttackRoot.h"
#include "FallAttack.h"
#include "RightJobPunch.h"

/// objs
#include "GameCamera/GameCamera.h"
#include "Player/Player.h"
#include"AttackEffect/AttackEffect.h"

// offscreen
#include "PostEffect/PostEffectRenderer.h"

/// frame
#include "Frame/Frame.h"

// 初期化
ThrustPunch::ThrustPunch(Player* player)
    : BaseComboAattackBehavior("ThrustPunch", player) {

    Init();
}

ThrustPunch::~ThrustPunch() {
}

void ThrustPunch::Init() {

    BaseComboAattackBehavior::Init();
   
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

    pPlayer_->GetPlayerCollisionInfo()->ResetComboEffect();
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
       
        pPlayer_->GetPlayerCollisionInfo()->ChangeAttackType(PlayerCollisionInfo::AttackType::THRUST);
        pPlayer_->GetPlayerCollisionInfo()->SetPosition(pPlayer_->GetWorldPosition());

        order_ = Order::LPUNCH;

        break;
        ///-----------------------------------------------------------------------------------------------------
        /// 左パンチ
        ///-----------------------------------------------------------------------------------------------------
    case Order::LPUNCH:

        // レール更新と座標反映
        pPlayer_->GetLeftHand()->RailForthComboUpdate(pPlayer_->GetPlayerCollisionInfo()->GetAttackSpeed(pPlayer_->GetLeftHand()->GetRailRunSpeedForth()));
       
        // イージング終了時の処理
        if (thrustRailManager_->GetRailMoveTime() < 1.0f) {
            break;
        }

        thrustRailManager_->SetRailMoveTime(1.0f);
        pPlayer_->GetLeftHand()->RailForthComboUpdate(0.0f);
        order_ = Order::LBACKPUNCH;

        break;

        ///-----------------------------------------------------------------------------------------------------
        /// 左パンチ(戻る)
        ///-----------------------------------------------------------------------------------------------------
    case Order::LBACKPUNCH:

        // レール更新と座標反映
        pPlayer_->GetLeftHand()->RailForthComboUpdate(pPlayer_->GetPlayerCollisionInfo()->GetAttackSpeed(-pPlayer_->GetLeftHand()->GetRailRunSpeedForth()));

        /// コンボ先行予約
        BaseComboAattackBehavior::PreOderNextComboForButton();

        // イージング終了時の処理
        if (thrustRailManager_->GetRailMoveTime() > 0.0f) {
            break;
        }

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
        pPlayer_->SetHeadScale(Vector3::OneVector());

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
    if (pPlayer_->GetPlayerCollisionInfo()->GetIsSlow() && !istimeSlow_) {
      /*  PostEffectRenderer::GetInstance()->SetPostEffectMode(PostEffectMode::GRAY);*/
        pPlayer_->SoundStrongPunch();
        pPlayer_->GetAttackEffect()->PlayHitStop("PlayerThurstAttackHitStop");
      /*  pPlayer_->GetAttackEffect()->PlayPostEffect("PlayerThurstAttackGrayScale");*/
        pPlayer_->GetGameCamera()->PlayAnimation("PlayerthrustAttack");
        pPlayer_->GetGameCamera()->PlayShake("ThurstAttackCamera");
        istimeSlow_ = true;
    }
}
