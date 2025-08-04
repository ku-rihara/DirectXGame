/// behavior
#include "TitleLeftPunch.h"
#include "TitleWait.h"

/// objs
#include "Player/Player.h"

/// input
#include"input/Input.h"

/// frame
#include "Frame/Frame.h"

// 初期化
TitleLeftPunch::TitleLeftPunch(Player* player)
    : BaseTitleBehavior("TitleLeftPunch", player) {

    ///---------------------------------------------------------
    /// 変数初期化
    ///---------------------------------------------------------

    /// parm
   /* punchEase_.time = 0.0f;*/
    waitTine_       = 0.0f;

    /// パンチ座標セット
    lHandStartPos_  = pPlayer_->GetLeftHand()->GetObjTransform().translation_;
    lHandTargetPos_ = pPlayer_->GetLeftHand()->GetObjTransform().LookAt(Vector3::ToForward()) * 2.0f;

    /// land
  
    EasingInit();

    // 振る舞い順序初期化
    order_ = Order::PUNCH;
}

TitleLeftPunch::~TitleLeftPunch() {
}

// 更新
void TitleLeftPunch::Update() {
    
    /// スケール変化
    scalingEase_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetScale(tempScale_);
 
    switch (order_) {

    case Order::PUNCH:

        ///----------------------------------------------------
        /// パンチ
        ///----------------------------------------------------

      punchEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->GetLeftHand()->SetObjTranslate(punchPosition_);

        break;

    case Order::BACKPUNCH:
        ///----------------------------------------------------
        /// バックパンチ
        ///----------------------------------------------------

        backPunchEase_.Update(Frame::DeltaTimeRate());
        pPlayer_->GetLeftHand()->SetObjTranslate(punchPosition_);
      
        break;

    case Order::WAIT:
        waitTine_ += Frame::DeltaTime();

        /// コンボ途切れ
        if (waitTine_ >= pPlayerParameter_->GetNormalComboParm(SECOND).waitTime) {
            pPlayer_->ChangeTitleBehavior(std::make_unique<TitleWait>(pPlayer_));
        }

        break;
    }
}

void TitleLeftPunch::EasingInit() {
    scalingEase_.Init("TitleScaling");
    scalingEase_.SetAdaptValue(&tempScale_);
    scalingEase_.Reset();


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

void TitleLeftPunch::Debug() {
}