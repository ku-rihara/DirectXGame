/// behavior
#include "RightJobPunch.h"
#include "ComboAttackRoot.h"
#include "GameCamera/GameCamera.h"
#include "LeftJobPunch.h"

/// objs
#include "LockOn/LockOn.h"
#include "MathFunction.h"
#include "Player/Player.h"

/// input
#include "input/Input.h"

/// frame
#include "Frame/Frame.h"

#include <imgui.h>

// 初期化
RightJobPunch::RightJobPunch(Player* player)
    : BaseComboAattackBehavior("RightJobPunch", player) {

    Init();
}

RightJobPunch::~RightJobPunch() {
}

void RightJobPunch::Init() {

    BaseComboAattackBehavior::Init();

    /// parm
    speed_    = pPlayerParameter_->GetParamaters().rushDistance;
    waitTine_ = 0.0f;

    /// 　突進の動き
    initPos_          = pPlayer_->GetWorldPosition();
    forwardDirection_ = pPlayer_->GetTransform().LookAt(Vector3::ToForward());
    rushPos_          = initPos_ + (forwardDirection_ * speed_); // 突進座標を決める

    /// collisionType
    pPlayer_->GetAttackController()->ChangeAttackType(PlayerAttackController::AttackType::NORMAL);

    EasingInit();

    // 　モーション
    BaseComboAattackBehavior::AnimationInit();

    // 振る舞い順序初期化
    order_ = Order::INIT;
}

// 更新
void RightJobPunch::Update() {

    // 　モーション
    BaseComboAattackBehavior::RotateMotionUpdate(0, GetRotateValue(), false);
    BaseComboAattackBehavior::FloatAnimationUpdate();

    /// スケール変化
    BaseComboAattackBehavior::ScalingEaseUpdate();

    // 攻撃中の移動
    pPlayer_->Move(pPlayerParameter_->GetParamaters().moveSpeed);

    switch (order_) {

        
        ///-----------------------------------------------------------------------------
        /// 初期化
        ///------------------------------------------------------------------------------
    case Order::INIT:
        pPlayer_->GetGameCamera()->PlayAnimation("RightPunch");
        order_ = Order::RUSH;
        break;

        ///-----------------------------------------------------------------------------
        /// 突進
        ///------------------------------------------------------------------------------
    case Order::RUSH:

        ChangeSpeedForLockOn(); // ロックオンによる突進スピードの変化

        rushEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->SetWorldPosition(tempRushPos_);

        break;

        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------
    case Order::PUNCH:
      
        /// パンチイージング更新
        punchEase_.Update(atkSpeed_);
        pPlayer_->GetRightHand()->SetObjTranslate(punchPosition_);

        /// 当たり判定座標
        pPlayer_->GetAttackController()->SetPosition(pPlayer_->GetRightHand()->GetObjTransform().GetWorldPos());

        break;

        ///----------------------------------------------------
        /// バックパンチ
        ///----------------------------------------------------
    case Order::BACKPUNCH:
  
        BaseComboAattackBehavior::PreOderNextComboForButton();
        pPlayer_->AdaptRotate();

        /// バックパンチイージング更新
        backPunchEase_.Update(atkSpeed_);
        pPlayer_->GetRightHand()->SetObjTranslate(punchPosition_);

        break;

    case Order::WAIT:
        waitTine_ += Frame::DeltaTime();
        pPlayer_->AdaptRotate();

        /// コンボ途切れ
        if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(FIRST).waitTime) {

            pPlayer_->ChangeComboBehavior(std::make_unique<ComboAttackRoot>(pPlayer_));
        } else {
            /// ボタンで次のコンボ
            BaseComboAattackBehavior::PreOderNextComboForButton();
            if (isNextCombo_) {
                BaseComboAattackBehavior::ChangeNextCombo(std::make_unique<LeftJobPunch>(pPlayer_));
            }
        }
    }
}

void RightJobPunch::EasingInit() {

    // punch Rush
    rushEase_.Init("PlayerPunchRush");
    rushEase_.SetAdaptValue(&tempRushPos_);
    rushEase_.Reset();

    rushEase_.SetStartValue(initPos_);
    rushEase_.SetEndValue(rushPos_);

    rushEase_.SetOnFinishCallback([this]() {
        rHandStartPos_  = pPlayer_->GetRightHand()->GetObjTransform().translation_;
        rHandTargetPos_ = pPlayer_->GetRightHand()->GetObjTransform().LookAt(Vector3::ToForward()) * pPlayerParameter_->GetNormalComboParm(FIRST).attackReach;

        // ease start end Set
        punchEase_.SetStartValue(rHandStartPos_);
        punchEase_.SetEndValue(rHandTargetPos_);

        backPunchEase_.SetStartValue(rHandTargetPos_);
        backPunchEase_.SetEndValue(rHandStartPos_);

        // 音
        pPlayer_->SoundPunch();
        order_ = Order::PUNCH;
    });

    // Punch
    punchEase_.Init("PlayerRightPunch");
    punchEase_.SetAdaptValue(&punchPosition_);
    punchEase_.Reset();

    punchEase_.SetOnFinishCallback([this]() {
        order_ = Order::BACKPUNCH;
    });

    // backPunch
    backPunchEase_.Init("PlayerRightBackPunch");
    backPunchEase_.SetAdaptValue(&punchPosition_);
    backPunchEase_.Reset();

    backPunchEase_.SetOnFinishCallback([this]() {
        order_ = Order::WAIT;
    });
}

void RightJobPunch::CollisionBoxInit() {
}

void RightJobPunch::ChangeSpeedForLockOn() {
    // ターゲット追い越し防止
    if (!(pPlayer_->GetLockOn() && pPlayer_->GetLockOn()->GetEnemyTarget())) {
        return;
    }

    Vector3 differectialVector = pPlayer_->GetLockOn()->GetCurrentTargetPosition() - pPlayer_->GetWorldPosition();
    // 距離
    float distance = differectialVector.Length();
    // 距離しきい値
    const float threshold = 2.2f;

    // しきい値より離れていない場合は終了
    if (distance <= threshold) {
        return;
    }

    // Y軸回り角度
    pPlayer_->SetRotationY(std::atan2(differectialVector.x, differectialVector.z));
    // しきい値を超える速さなら補正する
    if (speed_ > distance - threshold) {
        speed_ = distance - threshold;
    }
}

void RightJobPunch::Debug() {
    ImGui::Text("RightJobPunch");
}
