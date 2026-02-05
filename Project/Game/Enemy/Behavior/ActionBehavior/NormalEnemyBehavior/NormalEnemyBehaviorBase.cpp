#include "NormalEnemyBehaviorBase.h"
#include "Enemy/Types/NormalEnemy.h"

NormalEnemyBehaviorBase::NormalEnemyBehaviorBase(const std::string& name, NormalEnemy* enemy)
    : BaseEnemyBehavior(name, enemy) {
}

NormalEnemy* NormalEnemyBehaviorBase::GetEnemy() const {
    return static_cast<NormalEnemy*>(pBaseEnemy_);
}
