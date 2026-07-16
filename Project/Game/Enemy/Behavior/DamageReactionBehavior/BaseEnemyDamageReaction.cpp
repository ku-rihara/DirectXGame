#include "BaseEnemyDamageReaction.h"
#include "Enemy/Types/BaseEnemy.h"
#include "EnemyDeath.h"
#include <memory>

BaseEnemyDamageReaction::BaseEnemyDamageReaction(const std::string& name, BaseEnemy* boss) {

    name_       = name;
    pBaseEnemy_ = boss;
}

bool BaseEnemyDamageReaction::TryTransitionToExplosiveDeath() {
    if (pBaseEnemy_->GetHP() > 0.0f) {
        return false;
    }

    // コリジョンを無効化して死亡確定
    pBaseEnemy_->SetIsAdaptCollision(false);
    pBaseEnemy_->SetIsDeathPending(true);
    // 起き上がりを行わず爆散演出から死亡Behaviorへ移行
    pBaseEnemy_->ChangeDamageReactionBehavior(std::make_unique<EnemyDeath>(pBaseEnemy_, true));
    return true;
}