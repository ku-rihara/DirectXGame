#pragma once

#include "BaseEnemyDamageReaction.h"

class PlayerCollisionInfo;
class EnemyDamageReactionRoot : public BaseEnemyDamageReaction {
public:
    // コンストラクタ
    EnemyDamageReactionRoot(BaseEnemy* boss);
    ~EnemyDamageReactionRoot();

    void Update() override;
    void Debug() override;

    void SelectDamageActionBehaviorByAttack(const PlayerCollisionInfo&playerCollisionInfo);

private:
    PlayerCollisionInfo* pPlayerCollisionInfo_ = nullptr;
};
