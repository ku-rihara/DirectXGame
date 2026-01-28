#include "BasePlayerBehavior.h"
#include "Player/Player.h"

BasePlayerBehavior::BasePlayerBehavior(const std::string& name, Player* player)
    : BaseBehavior<Player>(name, player) {
    pPlayerParameter_ = player->GetParameter();
}