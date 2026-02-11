#include "EnemyAttackCollisionBox.h"
#include "Enemy/Types/BaseEnemy.h"
#include "Player/Player.h"
#include "Frame/Frame.h"

void EnemyAttackCollisionBox::Init() {
    SetIsAbleCollision(false);
    isHit_ = false;
    isFinish_ = false;
    adaptTimer_ = 0.0f;
    attackValue_ = 0.0f;
    BaseAABBCollisionBox::Init();
}

void EnemyAttackCollisionBox::Update() {
    BaseAABBCollisionBox::Update();
}

void EnemyAttackCollisionBox::TimerUpdate(float timeSpeed) {
    if (!isAbleCollision_) {
        return;
    }

    adaptTimer_ -= timeSpeed;

    if (adaptTimer_ > 0.0f) {
        return;
    }

    // タイマー終了
    SetIsAbleCollision(false);
    isFinish_ = true;
}

void EnemyAttackCollisionBox::AttackStart(float attackValue, const Vector3& collisionSize, const Vector3& offsetPos, float adaptTime) {
    // パラメータ設定
    attackValue_ = attackValue;
    adaptTimer_ = adaptTime;
    offsetPos_ = offsetPos;

    // 初期化
    isHit_ = false;
    isFinish_ = false;

    // サイズとオフセット設定
    SetSize(collisionSize);
    UpdateOffset();

    // コリジョン有効化
    SetIsAbleCollision(true);
}

void EnemyAttackCollisionBox::UpdateOffset() {
    SetOffset(offsetPos_);
}

void EnemyAttackCollisionBox::OnCollisionEnter([[maybe_unused]] BaseCollider* other) {
    if (dynamic_cast<Player*>(other)) {
        isHit_ = true;
    }
}

void EnemyAttackCollisionBox::OnCollisionStay([[maybe_unused]] BaseCollider* other) {
    if (dynamic_cast<Player*>(other)) {
        isHit_ = true;
    }
}

Vector3 EnemyAttackCollisionBox::GetCollisionPos() const {
    return BaseAABBCollisionBox::GetCollisionPos();
}
