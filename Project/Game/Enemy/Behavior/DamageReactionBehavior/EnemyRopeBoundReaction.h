#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
#include "Vector3.h"

class SideRope;
class EnemyDamageRenditionData;

class EnemyRopeBoundReaction : public BaseEnemyDamageReaction {
public:
public:
    // コンストラクタ
    EnemyRopeBoundReaction(BaseEnemy* boss, EnemyDamageReactionData* reactionData,  SideRope* sideRope);
    ~EnemyRopeBoundReaction() override;

    void Update() override;
    void Debug() override;

private:
    EnemyDamageReactionData* pReactionData_ = nullptr;
    SideRope* pSideRope_                    = nullptr;
};