/// behaviorTitleRightPunch
#include "TitleRightPunch.h"
#include "TitleLeftPunch.h"
/// objs
#include "LockOn/LockOn.h"
#include "Player/Player.h"

/// input
#include "input/Input.h"

/// frame
#include "Frame/Frame.h"

#include <imgui.h>

// 初期化
TitleRightPunch::TitleRightPunch(Player* player)
    : BaseTitleBehavior("TitleRightPunch", player) {

    ///---------------------------------------------------------
    /// 変数初期化
    ///---------------------------------------------------------


    waitTine_       = 0.0f;

    /// land
   /* startEasing_.maxTime   = 0.5f;
    startEasing_.amplitude = 0.6f;
    startEasing_.period    = 0.2f;*/

    rHandStartPos_  = pPlayer_->GetRightHand()->GetTransform().translation_;
    rHandTargetPos_ = pPlayer_->GetRightHand()->GetTransform().LookAt(Vector3::ToForward()) * 2.0f;

    EasingInit();

    // 振る舞い順序初期化
    order_ = Order::PUNCH;
}

TitleRightPunch::~TitleRightPunch() {
}

// 更新
void TitleRightPunch::Update() {
    /// スケール変化
    scalingEase_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetScale(tempScale_);

    switch (order_) {

    case Order::PUNCH:
        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------

         punchEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->GetRightHand()->SetWorldPosition(punchPosition_);

       
        break;

    case Order::BACKPUNCH:
        ///----------------------------------------------------
        /// バックパンチ
        ///----------------------------------------------------
        backPunchEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->GetRightHand()->SetWorldPosition(punchPosition_);
       
        break;

    case Order::WAIT:
        waitTine_ += Frame::DeltaTime();

        /// コンボ途切れ
        if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(FIRST).waitTime) {
            pPlayer_->ChangeTitleBehavior(std::make_unique<TitleLeftPunch>(pPlayer_));
        }
    }
}

void TitleRightPunch::EasingInit() {
    scalingEase_.Init("TitleScaling");
    scalingEase_.SetAdaptValue(&tempScale_);
    scalingEase_.Reset();

    punchEase_.Init("PlayerRightPunch");
    punchEase_.SetAdaptValue(&punchPosition_);
    punchEase_.Reset();

    punchEase_.SetOnFinishCallback([this]() {
        order_ = Order::BACKPUNCH;
    });

    backPunchEase_.Init("PlayerRightBackPunch");
    backPunchEase_.SetAdaptValue(&punchPosition_);
    backPunchEase_.Reset();

    backPunchEase_.SetOnFinishCallback([this]() {
        order_ = Order::WAIT;
    });

    // start end Value Set
    punchEase_.SetStartValue(rHandStartPos_);
    punchEase_.SetEndValue(rHandTargetPos_);

    backPunchEase_.SetStartValue(rHandTargetPos_);
    backPunchEase_.SetEndValue(rHandStartPos_);
}

void TitleRightPunch::Debug() {
    ImGui::Text("TitleRightPunch");
}
