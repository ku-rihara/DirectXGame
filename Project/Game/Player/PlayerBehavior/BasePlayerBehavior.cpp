#include "BasePlayerBehavior.h"
#include"Player/Player.h"


BasePlayerBehavior::BasePlayerBehavior(const std::string& name, Player* player) {
    name_             = name;
    pPlayer_          = player;
    pPlayerParameter_ = player->GetParameter();
}