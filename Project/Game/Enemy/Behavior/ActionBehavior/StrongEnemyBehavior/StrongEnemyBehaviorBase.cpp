#include "StrongEnemyBehaviorBase.h"
#include "Enemy/Types/StrongEnemy.h"

StrongEnemyBehaviorBase::StrongEnemyBehaviorBase(const std::string& name, StrongEnemy* enemy)
    : BaseEnemyBehavior(name, enemy) {
}

StrongEnemy* StrongEnemyBehaviorBase::GetEnemy() const {
    return static_cast<StrongEnemy*>(pBaseEnemy_);
}
