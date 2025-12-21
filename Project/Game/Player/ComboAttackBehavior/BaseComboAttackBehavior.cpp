#include "BaseComboAttackBehavior.h"
#include "Frame/Frame.h"
#include "Player/Player.h"
#include"input/Input.h"

BaseComboAttackBehavior::BaseComboAttackBehavior(const std::string& name, Player* player) {
    name_             = name;
    pPlayer_          = player;
    pPlayerParameter_ = player->GetParameter();
  
}


///  コンボ移動処理
void BaseComboAttackBehavior::ChangeNextCombo(std::unique_ptr<BaseComboAttackBehavior> nextCombo) {

    pPlayer_->ChangeComboBehavior(std::move(nextCombo));
}

