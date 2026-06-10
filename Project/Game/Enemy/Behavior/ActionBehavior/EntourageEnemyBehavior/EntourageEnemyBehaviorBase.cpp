#include "EntourageEnemyBehaviorBase.h"
#include "Enemy/Types/EntourageEnemy.h"

EntourageEnemyBehaviorBase::EntourageEnemyBehaviorBase(const std::string& name, EntourageEnemy* enemy)
    : BaseEnemyBehavior(name, enemy) {
}

EntourageEnemy* EntourageEnemyBehaviorBase::GetEnemy() const {
    return static_cast<EntourageEnemy*>(pBaseEnemy_);
}
