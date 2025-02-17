#include "BaseComboAattackBehavior.h"
#include"JoyState/JoyState.h"
#include"Player/Player.h"
#include"Frame/Frame.h"


/// <summary>
///  コンボ移動フラグ処理
/// </summary>
/// <param name="nextCombo"></param>
void BaseComboAattackBehavior::PreOderNextComboForButton() {
  
    if (Input::GetInstance()->TrrigerKey(DIK_H)) {
        isNextCombo_ = true;
     /*   pPlayer_->ChangeComboBehavior(std::move(nextCombo));*/
    }
    else {
        if (!(Input::GetInstance()->GetJoystickState(0, joyState))) return;

        if (!((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X))) return;

        isNextCombo_ = true;
    }
}

/// <summary>
///  コンボ移動処理
/// </summary>
/// <param name="nextCombo"></param>
void BaseComboAattackBehavior::ChangeNextCombo(std::unique_ptr<BaseComboAattackBehavior> nextCombo) {
    if (!isNextCombo_)return;
           pPlayer_->ChangeComboBehavior(std::move(nextCombo));
         
}

void BaseComboAattackBehavior::AnimationInit() {
    //rotate
    rotateEaseT_ = 0.0f;
    tempRotateValue_ = 0.0f;
    pPlayer_->SetHeadRotateY(0.0f);
    rotateValue_ = pPlayer_->GetPlayerParams().attackRotate;
    //scaling
    startEasing_.time = 0.0f;
    pPlayer_->SetScale(Vector3::UnitVector());
    startEasing_.maxTime = 0.5f;
    startEasing_.amplitude = 0.6f;
    startEasing_.period = 0.2f;
 
}

void BaseComboAattackBehavior::ScalingEaseUpdate() {
    startEasing_.time += Frame::DeltaTimeRate();
    startEasing_.time = std::min(startEasing_.time, startEasing_.maxTime);
    pPlayer_->SetScale(EaseAmplitudeScale(Vector3::UnitVector(), startEasing_.time, startEasing_.maxTime,
        startEasing_.amplitude, startEasing_.period));
}

void BaseComboAattackBehavior::RotateMotionUpdate(const bool& isClockwise) {
    rotateEaseT_ += Frame::DeltaTimeRate();
    tempRotateValue_ = EaseInSine(0.0f, rotateValue_, rotateEaseT_, pPlayer_->GetPlayerParams().attackRotateEaseT);

    if (isClockwise) {
        pPlayer_->SetHeadRotateY(tempRotateValue_);
    } else {
        pPlayer_->SetHeadRotateY(-tempRotateValue_);
    }

    if (rotateEaseT_ < pPlayer_->GetPlayerParams().attackRotateEaseT) return;
    rotateEaseT_ = pPlayer_->GetPlayerParams().attackRotateEaseT;
    pPlayer_->SetHeadRotateY(0.0f);
}

void BaseComboAattackBehavior::FloatAnimationUpdate() {

}