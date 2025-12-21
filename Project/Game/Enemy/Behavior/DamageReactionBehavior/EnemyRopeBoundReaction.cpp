#include "EnemyRopeBoundReaction.h"
#include"Field/SideRope/SideRopeReboundSystem.h"
#include "Field/SideRope/SideRope.h"
#include"Enemy/Types/BaseEnemy.h"

// 初期化
EnemyRopeBoundReaction::EnemyRopeBoundReaction(
    BaseEnemy* boss,
    Vector3 knockBackVelocity,
    SideRope* sideRope)
    : BaseEnemyDamageReaction("EnemyRopeBoundReaction", boss) {

    knockBackVelocity_ = knockBackVelocity;
    pSideRope_         = sideRope;

     if (pSideRope_ && pBaseEnemy_) {
        pSideRope_->Rebound(pBaseEnemy_, knockBackVelocity_);
    }
}

EnemyRopeBoundReaction::~EnemyRopeBoundReaction() {
}

void EnemyRopeBoundReaction::Update(float deltaTime) {
    if (!pBaseEnemy_) {
        return;
    }

    // ロープ反発状態を取得して更新
    SideRopeReboundSystem& state = pBaseEnemy_->GetSideRopeReboundSystem();

    if (state.IsActive()) {
        // 反発速度を計算して適用
        Vector3 reboundVelocity = state.Update(deltaTime);
        pBaseEnemy_->AddPosition(reboundVelocity);
    } else {
        // 反発完了したらダメージルートに戻る
        pBaseEnemy_->BackToDamageRoot();
    }
}

void EnemyRopeBoundReaction::Debug() {
}
