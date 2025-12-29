#include "BaseComboAttackBehavior.h"
#include "Frame/Frame.h"
#include "input/Input.h"
#include "Player/Player.h"

BaseComboAttackBehavior::BaseComboAttackBehavior(const std::string& name, Player* player)
    : BaseBehavior<Player>(name, player) {
    pPlayerParameter_ = player->GetParameter();
}

void BaseComboAttackBehavior::ChangeNextCombo(std::unique_ptr<BaseComboAttackBehavior> nextCombo) {
    pOwner_->ChangeComboBehavior(std::move(nextCombo));
}