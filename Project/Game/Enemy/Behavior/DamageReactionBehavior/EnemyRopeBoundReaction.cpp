#include "EnemyRopeBoundReaction.h"
#include "Field/SideRope/SideRope.h"

// 初期化
EnemyRopeBoundReaction::EnemyRopeBoundReaction(
    BaseEnemy* boss,
    Vector3 knockBackVelocity,
    SideRope* sideRope)
    : BaseEnemyDamageReaction("EnemyRopeBoundReaction", boss) {

    knockBackVelocity_ = knockBackVelocity;
    pSideRope_         = sideRope;
}

EnemyRopeBoundReaction::~EnemyRopeBoundReaction() {
}

void EnemyRopeBoundReaction::Update() {
    pSideRope_->Rebound(pBaseEnemy_);
}

void EnemyRopeBoundReaction::Debug() {
}
