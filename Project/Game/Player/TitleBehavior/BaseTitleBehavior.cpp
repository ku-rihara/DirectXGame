#include "BaseTitleBehavior.h"
#include "Player/Player.h"

BaseTitleBehavior::BaseTitleBehavior(const std::string& name, Player* player) {
    name_             = name;
    pPlayer_          = player;
    pPlayerParameter_ = player->GetParamater();
}
