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
    EnemyRopeBoundReaction(BaseEnemy* boss, Vector3 knockBackVelocity, SideRope* sideRope);
    ~EnemyRopeBoundReaction() override;

    void Update( float deltaTime) override;
    void Debug() override;

private:
    Vector3 knockBackVelocity_;
    SideRope* pSideRope_ = nullptr;
};