#include "BaseComboAattackBehavior.h"
#include "Frame/Frame.h"
#include "Player/Player.h"
#include"input/Input.h"

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
  
    tempRotateValue_ = 0.0f;
    pPlayer_->RotateReset();
    rotateValue_     = pPlayerParameter_->GetParamaters().attackRotate;
    rotateValueAnti_ = pPlayerParameter_->GetParamaters().attackRotateAnit;

    // scaling

    startEasing_.Init("AttackStart");
    startEasing_.SetAdaptValue(&tempScaleValue_);
    tempScaleValue_ = Vector3::UnitVector();

    rotateEasing_.Init("AttackRotate");
    rotateEasing_.SetAdaptValue(&tempRotateValue_);
    
    floatEase_.Init("AttackFloating");
    floatEase_.SetAdaptValue(&tempFloatValue_);

    floatEase_.SetOnFinishCallback([this] {
        floatEase_.SetCurrentValue(0.0f);
    });

     rotateEasing_.SetOnFinishCallback([this] {
        pPlayer_->SetRotationY(tempRotateValue_);
    });
    
}

void BaseComboAattackBehavior::ScalingEaseUpdate() {
    startEasing_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetHeadScale(tempScaleValue_);
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
    pPlayer_->SetHeadRotateY(tempRotateValue_);
}

void BaseComboAattackBehavior::FloatAnimationUpdate() {

    floatEase_.Update(Frame::DeltaTimeRate());
    pPlayer_->SetHeadPosY(tempFloatValue_);
}