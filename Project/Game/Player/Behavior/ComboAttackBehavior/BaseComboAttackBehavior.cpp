#include "BaseComboAttackBehavior.h"
#include "Player/Player.h"

BaseComboAttackBehavior::BaseComboAttackBehavior(const std::string& name, Player* player)
    : BaseBehavior<Player>(name, player) {
    pPlayerParameter_ = player->GetParameter();
}

void BaseComboAttackBehavior::ChangeNextCombo(std::unique_ptr<BaseComboAttackBehavior> nextCombo) {
    pOwner_->ChangeCombo(std::move(nextCombo));
}