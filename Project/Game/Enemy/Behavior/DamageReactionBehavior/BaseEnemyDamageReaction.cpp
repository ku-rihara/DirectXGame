#include "BaseEnemyDamageReaction.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"

BaseEnemyDamageReaction::BaseEnemyDamageReaction(const std::string& name, BaseEnemy* boss) {

    name_       = name;
    pBaseEnemy_ = boss;
}