#include "LeaderEnemyBehaviorBase.h"
#include "Enemy/Types/LeaderEnemy.h"

LeaderEnemyBehaviorBase::LeaderEnemyBehaviorBase(const std::string& name, LeaderEnemy* enemy)
    : BaseEnemyBehavior(name, enemy) {
}

LeaderEnemy* LeaderEnemyBehaviorBase::GetEnemy() const {
    return static_cast<LeaderEnemy*>(pBaseEnemy_);
}
