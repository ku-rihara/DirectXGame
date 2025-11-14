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

    if (Input::GetInstance()->TriggerKey(KeyboardKey::H)) {
        isNextCombo_ = true;
    } else {
        if (!(Input::IsTriggerPad(0, GamepadButton::X))) {
            return;
        }

        isNextCombo_ = true;
    }
}

///  コンボ移動処理
void BaseComboAattackBehavior::ChangeNextCombo(std::unique_ptr<BaseComboAattackBehavior> nextCombo) {
    if (!isNextCombo_)
        return;
    pPlayer_->ChangeComboBehavior(std::move(nextCombo));
}


void BaseComboAattackBehavior::Init() {
    atkSpeed_ = pPlayer_->GetPlayerCollisionInfo()->GetAttackSpeed(Frame::DeltaTimeRate());
}

