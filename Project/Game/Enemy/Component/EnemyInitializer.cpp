#include "EnemyInitializer.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "Enemy/Types/BaseEnemy.h"

EnemyInitializer::EnemyInitializer(BaseEnemy* enemy, const Vector3& spawnPos) {
    enemy_    = enemy;
    spawnPos_ = spawnPos;
}

void EnemyInitializer::Run() {
    ResetFlags();
    InitAnimator();
    InitBehaviorController();
    InitHP();
    InitUI();
    InitTransform();
    InitAttackCollision();
    InitEffects();
    InitDamageReaction();
    ApplyCollision();
}

void EnemyInitializer::ResetFlags() {
    enemy_->isDeath_          = false;
    enemy_->isDeathPending_   = false;
    enemy_->isCollisionRope_  = false;
    enemy_->isInAnticipation_ = false;
    enemy_->isAttacking_      = false;
}

void EnemyInitializer::InitAnimator() {
    std::string modelFolder;
    if (enemy_->baseInfo_.GetType() == BaseEnemy::Type::NORMAL) {
        modelFolder = "EntourageEnemy/";
    } else {
        modelFolder = "LeaderEnemy/";
    }
    enemy_->animator_.Init(modelFolder);
}

void EnemyInitializer::InitBehaviorController() {
    enemy_->behaviorCtrl_.Init(enemy_);
}

void EnemyInitializer::InitHP() {
    enemy_->HPMax_ = enemy_->baseInfo_.GetParameter().hpMax;
    enemy_->hp_    = enemy_->HPMax_;
}

void EnemyInitializer::InitUI() {
    if (!enemy_->enemyUIs_) {
        enemy_->enemyUIs_ = std::make_unique<EnemyUIs>();
    }
    enemy_->enemyUIs_->Init(enemy_->HPMax_);
}

void EnemyInitializer::InitTransform() {
    auto& transform          = enemy_->baseTransform_;
    transform.translation_   = spawnPos_;
    transform.translation_.y = enemy_->baseInfo_.GetParameter().basePosY;
    transform.SetAnchorRotation(Vector3(0.0f, 1.5f, 0.0f));
    transform.scale_ = Vector3::ZeroVector();
}

void EnemyInitializer::InitAttackCollision() {
    if (!enemy_->attackCollisionBox_) {
        enemy_->attackCollisionBox_ = std::make_unique<EnemyAttackCollisionBox>();
    }
    enemy_->attackCollisionBox_->Init();
    enemy_->attackCollisionBox_->SetEnemy(enemy_);
    enemy_->attackCollisionBox_->SetParentTransform(&enemy_->baseTransform_);
}

void EnemyInitializer::InitEffects() {
    if (!enemy_->enemyEffects_) {
        enemy_->enemyEffects_ = std::make_unique<EnemyEffects>();
    }
    enemy_->enemyEffects_->Init(&enemy_->baseTransform_);
}

void EnemyInitializer::InitDamageReaction() {
    enemy_->ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(enemy_));
}

void EnemyInitializer::ApplyCollision() {
    enemy_->RefreshCollision();
}
