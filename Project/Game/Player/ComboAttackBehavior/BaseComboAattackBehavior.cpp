#include "BaseComboAattackBehavior.h"
#include "Frame/Frame.h"
#include "JoyState/JoyState.h"
#include "Player/Player.h"

BaseComboAattackBehavior::BaseComboAattackBehavior(const std::string& name, Player* player) {
    name_ = name;
    pPlayer_ = player;
    pPlayerParameter_ = player->GetParamater();
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
  
    pPlayer_->SetHeadScale(Vector3::UnitVector());
   /* startEasing_.time      = 0.0f;
    startEasing_.maxTime   = 0.7f;
    startEasing_.amplitude = 1.5f;
    startEasing_.period    = 0.2f;*/

    /// floatmotion
    /*floatEase_.time      = 0.0f;
    floatEase_.backRatio = 0.8f;
    floatEase_.maxTime   = pPlayerParameter_->GetParamaters().attackFloatEaseT;*/
    floatValue_          = pPlayerParameter_->GetParamaters().attackFloatValue;
    tempFloatValue_      = 0.0f;
}

void BaseComboAattackBehavior::ScalingEaseUpdate() {
   /* startEasing_.time += Frame::DeltaTimeRate();
    startEasing_.time = std::min(startEasing_.time, startEasing_.maxTime);
    pPlayer_->SetHeadScale(EaseAmplitudeScale(Vector3::UnitVector(), startEasing_.time, startEasing_.maxTime,
        startEasing_.amplitude, startEasing_.period));*/
}

void BaseComboAattackBehavior::RotateMotionUpdate(const float& start, const float& end, const bool& isClockwise) {
  /*  rotateEaseT_ += Frame::DeltaTimeRate();
    float plus = isClockwise ? 1.0f : -1.0f;

    tempRotateValue_ = EaseInSine(start, end * plus, rotateEaseT_, pPlayerParameter_->GetParamaters().attackRotateEaseT);
    pPlayer_->SetHeadRotateY(tempRotateValue_);

    if (rotateEaseT_ < pPlayerParameter_->GetParamaters().attackRotateEaseT)
        return;
    rotateEaseT_ = pPlayerParameter_->GetParamaters().attackRotateEaseT;*/
}

void BaseComboAattackBehavior::FloatAnimationUpdate() {
   /* floatEase_.time += Frame::DeltaTimeRate();
    tempFloatValue_ = Back::OutQuintZero(0.0f, floatValue_, floatEase_.time, floatEase_.maxTime, floatEase_.backRatio);
    pPlayer_->SetHeadPosY(tempFloatValue_);
    if (floatEase_.time < floatEase_.maxTime)
        return;
    floatEase_.time = floatEase_.maxTime;
    pPlayer_->SetHeadPosY(0.0f);*/
}