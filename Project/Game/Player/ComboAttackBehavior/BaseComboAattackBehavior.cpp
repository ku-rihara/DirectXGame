#include "BaseComboAattackBehavior.h"
#include"JoyState/JoyState.h"
#include"Player/Player.h"


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