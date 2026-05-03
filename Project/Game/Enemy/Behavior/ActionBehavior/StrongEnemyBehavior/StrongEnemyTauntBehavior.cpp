#include "StrongEnemyTauntBehavior.h"

#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemyWait.h"
#include "Enemy/Types/StrongEnemy.h"

StrongEnemyTauntBehavior::StrongEnemyTauntBehavior(StrongEnemy* enemy)
    : StrongEnemyBehaviorBase("StrongEnemyTauntBehavior", enemy) {

    pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Taunt, true);
}

void StrongEnemyTauntBehavior::Update() {
    pBaseEnemy_->DirectionToPlayer();

    float tauntRange = GetEnemy()->GetStrongParameter().tauntRange;
    if (pBaseEnemy_->CalcDistanceToPlayer() > tauntRange) {
        pBaseEnemy_->ChangeBehavior(std::make_unique<EnemyWait>(pBaseEnemy_));
    }
}

void StrongEnemyTauntBehavior::Debug() {
}