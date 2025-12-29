#include "EnemyRopeBoundReaction.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Field/SideRope/SideRope.h"
#include "Field/SideRope/SideRopeReboundSystem.h"

// 初期化
EnemyRopeBoundReaction::EnemyRopeBoundReaction(
    BaseEnemy* boss,
    Vector3 knockBackVelocity,
    SideRope* sideRope)
    : BaseEnemyDamageReaction("EnemyRopeBoundReaction", boss) {

    knockBackVelocity_ = knockBackVelocity;
    pSideRope_         = sideRope;
    ropeReboundSystem_ = std::make_unique<SideRopeReboundSystem>();
    
    jumpSpeed_ = pBaseEnemy_->GetParameter().ropeReboundJumpValue;

    // ロープの法線方向を計算
    Vector3 ropeNormal = pSideRope_->CalculateRopeNormal();

    // 敵のロープ反発状態を初期化
    ropeReboundSystem_->Init(knockBackVelocity_, ropeNormal, pSideRope_->GetRopeParam());
}

EnemyRopeBoundReaction::~EnemyRopeBoundReaction() {
}

void EnemyRopeBoundReaction::Update(float deltaTime) {
    if (!pBaseEnemy_) {
        return;
    }

    // 反発速度を計算して適用
    Vector3 reboundVelocity = ropeReboundSystem_->Update(deltaTime);
    reboundVelocity.y       = 0.0f;
    pBaseEnemy_->AddPosition(reboundVelocity);
    FallUpdate();

    // 着地とロープ跳ね返りが終わったらダメージルートに戻る
    if (!isLanding_ || ropeReboundSystem_->IsActive()) {
        return;
    }

    // 反発完了したらダメージルートに戻る
    pBaseEnemy_->BackToDamageRoot();
}

void EnemyRopeBoundReaction::Debug() {
}

void EnemyRopeBoundReaction::FallUpdate() {
    // 落下を適応
    if (ropeReboundSystem_->GetPhase() != SideRopeReboundSystem::Phase::STRETCH) {
        float gravity        = pBaseEnemy_->GetParameter().ropeReboundGravity;
        float fallSpeedLimit = pBaseEnemy_->GetParameter().ropeReboundFallSpeedLimit;
        pBaseEnemy_->Jump(jumpSpeed_, fallSpeedLimit, gravity);

        if (pBaseEnemy_->GetWorldPosition().y <= 0.0f) {
            Reset();
            isLanding_ = true;
        }
    }
}

void EnemyRopeBoundReaction::Reset() {
    pBaseEnemy_->SetWorldPositionY(pBaseEnemy_->GetParameter().basePosY);
    pBaseEnemy_->SetBodyRotateX(0.0f);
 }