#include "BaseComboAattackBehavior.h"
#include"JoyState/JoyState.h"
#include"Player/Player.h"


/// <summary>
///  コンボ移動処理
/// </summary>
/// <param name="nextCombo"></param>
void BaseComboAattackBehavior::ChangeNextComboForButton(std::unique_ptr<BaseComboAattackBehavior> nextCombo) {
  
    if (Input::GetInstance()->TrrigerKey(DIK_H)) {
        pPlayer_->ChangeComboBehavior(std::move(nextCombo));
    }
    else {
        if (!(Input::GetInstance()->GetJoystickState(0, joyState))) return;

        if (!((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X))) return;

        pPlayer_->ChangeComboBehavior(std::move(nextCombo));
    }
}