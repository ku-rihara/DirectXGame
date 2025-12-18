#include "EnemyRopeBoundReaction.h"
#include "Field/SideRope/SideRope.h"

// 初期化
EnemyRopeBoundReaction::EnemyRopeBoundReaction(
    BaseEnemy* boss,
    EnemyDamageReactionData* reactionData,
     SideRope* sideRope)
    : BaseEnemyDamageReaction("EnemyRopeBoundReaction", boss) {

    pReactionData_ = reactionData;
    pSideRope_     = sideRope;
}

EnemyRopeBoundReaction::~EnemyRopeBoundReaction() {
}

void EnemyRopeBoundReaction::Update() {
    pSideRope_->Rebound(pBaseEnemy_);
}

void EnemyRopeBoundReaction::Debug() {
}
