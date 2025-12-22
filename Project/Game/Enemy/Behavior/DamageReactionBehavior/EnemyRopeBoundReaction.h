#pragma once

#include "BaseEnemyDamageReaction.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
#include "Field/SideRope/SideRopeReboundSystem.h"
#include "Vector3.h"
#include <memory>

class SideRope;
class EnemyDamageRenditionData;

class EnemyRopeBoundReaction : public BaseEnemyDamageReaction {
public:
public:
    // コンストラクタ
    EnemyRopeBoundReaction(BaseEnemy* boss, Vector3 knockBackVelocity, SideRope* sideRope);
    ~EnemyRopeBoundReaction() override;

    void Update(float deltaTime) override;
    void FallUpdate();

    void Reset();
    void Debug() override;

private:
    std::unique_ptr<SideRopeReboundSystem> ropeReboundSystem_;
    Vector3 knockBackVelocity_;
    SideRope* pSideRope_ = nullptr;
    float jumpSpeed_     = 0.0f;
    bool isLanding_      = false;
};