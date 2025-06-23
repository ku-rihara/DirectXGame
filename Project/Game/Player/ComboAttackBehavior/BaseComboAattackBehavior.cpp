#include "BaseComboAattackBehavior.h"
#include "Frame/Frame.h"
#include "JoyState/JoyState.h"
#include "Player/Player.h"

BaseComboAattackBehavior::BaseComboAattackBehavior(const std::string& name, Player* player) {
    name_             = name;
    pPlayer_          = player;
    pPlayerParameter_ = player->GetParameter();
}

///  コンボ移動フラグ処理
void BaseComboAattackBehavior::PreOderNextComboForButton() {

    if (Input::GetInstance()->TrrigerKey(DIK_H)) {
        isNextCombo_ = true;
    } else {
        if (!(Input::IsTriggerPad(0, XINPUT_GAMEPAD_X)))
            return;

        isNextCombo_ = true;
    }
}

///  コンボ移動処理
void BaseComboAattackBehavior::ChangeNextCombo(std::unique_ptr<BaseComboAattackBehavior> nextCombo) {
    if (!isNextCombo_)
        return;
    pPlayer_->ChangeComboBehavior(std::move(nextCombo));
}

void BaseComboAattackBehavior::AnimationInit() {
    // rotate
    rotateEaseT_     = 0.0f;
    tempRotateValue_ = 0.0f;
    pPlayer_->SetRotateInit();
    rotateValue_     = pPlayerParameter_->GetParamaters().attackRotate;
    rotateValueAnti_ = pPlayerParameter_->GetParamaters().attackRotateAnit;

    // scaling

    startEasing_.Init("AttackStart");
    startEasing_.ApplyFromJson("AttackStartScaling.json");
    startEasing_.SaveAppliedJsonFileName();
    startEasing_.SetAdaptValue(&tempScaleValue_);
    tempScaleValue_ = Vector3::UnitVector();

    rotateEasing_.Init("AttackFloating");
    rotateEasing_.ApplyFromJson("AttackStartScaling.json");
    rotateEasing_.SaveAppliedJsonFileName();
    rotateEasing_.SetAdaptValue(&tempRotateValue_);

    /// floatmotion
    /*floatEase_.time      = 0.0f;
    floatEase_.backRatio = 0.8f;
    floatEase_.maxTime   = pPlayerParameter_->GetParamaters().attackFloatEaseT;*/
    /*  floatValue_     = pPlayerParameter_->GetParamaters().attackFloatValue;
      tempFloatValue_ = 0.0f;*/

    floatEase_.Init("AttackRotate");
    floatEase_.ApplyFromJson("AttackStartScaling.json");
    floatEase_.SaveAppliedJsonFileName();
    floatEase_.SetAdaptValue(&tempFloatValue_);

    floatEase_.SetOnFinishCallback([this] {
        floatEase_.SetCurrentValue(0.0f);
    });
}

void BaseComboAattackBehavior::ScalingEaseUpdate() {
    startEasing_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetScale(tempScaleValue_);
}

void BaseComboAattackBehavior::RotateMotionUpdate(const float& start, const float& end, const bool& isClockwise) {

    if (isClockwise) {
        rotateEasing_.SetStartValue(start);
        rotateEasing_.SetEndValue(-end);
    } else {
        rotateEasing_.SetStartValue(start);
        rotateEasing_.SetEndValue(end);
    }

    rotateEasing_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetRotationY(tempRotateValue_);
}

void BaseComboAattackBehavior::FloatAnimationUpdate() {

    floatEase_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetHeadPosY(tempFloatValue_);

    /* floatEase_.time += Frame::DeltaTimeRate();
     tempFloatValue_ = Back::OutQuintZero(0.0f, floatValue_, floatEase_.time, floatEase_.maxTime, floatEase_.backRatio);
     pPlayer_->SetHeadPosY(tempFloatValue_);
     if (floatEase_.time < floatEase_.maxTime)
         return;
     floatEase_.time = floatEase_.maxTime;
     pPlayer_->SetHeadPosY(0.0f);*/
}