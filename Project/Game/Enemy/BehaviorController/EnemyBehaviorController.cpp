#include "EnemyBehaviorController.h"
// Enemy
#include "Enemy/Types/BaseEnemy.h"
// Behaviors
#include "Enemy/Behavior/ActionBehavior/BaseEnemyBehavior.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Enemy/Behavior/DamageReactionBehavior/BaseEnemyDamageReaction.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDeath.h"
// Player
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"

EnemyBehaviorController::~EnemyBehaviorController() = default;

///========================================================
/// 初期化
///========================================================
void EnemyBehaviorController::Init(BaseEnemy* owner) {
    pOwner_                  = owner;
    isDamageColling_         = false;
    damageCollTime_          = 0.0f;
    lastReceivedAttackName_.clear();
    moveBehavior_   = nullptr;
    damageBehavior_ = nullptr;
}

///========================================================
/// 毎フレーム更新
///========================================================
void EnemyBehaviorController::Update(float deltaTime) {
    // ダメージリアクション中でなければ移動ビヘイビアを更新
    if (moveBehavior_) {
        bool damageActive = damageBehavior_ && !dynamic_cast<EnemyDamageReactionRoot*>(damageBehavior_.get());
        if (!damageActive) {
            moveBehavior_->Update();
        }
    }

    DamageCollingUpdate(deltaTime);

    if (damageBehavior_) {
        damageBehavior_->Update(deltaTime);
    }
}

///========================================================
/// 移動ビヘイビア変更
///========================================================
void EnemyBehaviorController::ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior) {
    if (IsChangeLocked()) {
        return;
    }
    moveBehavior_ = std::move(behavior);
}

///========================================================
/// ダメージリアクションビヘイビア変更
///========================================================
void EnemyBehaviorController::ChangeDamageReactionBehavior(std::unique_ptr<BaseEnemyDamageReaction> behavior) {
    if (IsChangeLocked()) {
        return;
    }

    // スポーン中にダメージを受けた場合はスケール・コリジョンをリセット
    if (dynamic_cast<EnemySpawn*>(moveBehavior_.get())) {
        pOwner_->ScaleReset();
        pOwner_->OnSpawnCompleted();
        pOwner_->SetIsAdaptCollision(true);
    }

    damageBehavior_ = std::move(behavior);
}

///========================================================
/// プレイヤー攻撃コライダーとの衝突処理
///========================================================
void EnemyBehaviorController::OnPlayerAttackCollision(PlayerAttackCollider* attackController) {
    // 死亡中は受け付けない
    if (pOwner_->GetIsDeath() || pOwner_->GetIsDeathPending()) {
        return;
    }
    if (dynamic_cast<EnemyDeath*>(damageBehavior_.get())) {
        return;
    }

    if (!attackController->GetComboAttackData()) {
        return;
    }

    const std::string& attackName = attackController->GetComboAttackData()->GetGroupName();
    if (attackName.empty()) {
        return;
    }

    // 同一攻撃のクーリング中
    if (isDamageColling_ && lastReceivedAttackName_ == attackName) {
        return;
    }

    attackController->NotifyDamageHit();

    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(pOwner_));

    if (EnemyDamageReactionRoot* root = dynamic_cast<EnemyDamageReactionRoot*>(damageBehavior_.get())) {
        root->SelectDamageActionBehaviorByAttack(attackController);
    }
}

///========================================================
/// ダメージクーリング開始
///========================================================
void EnemyBehaviorController::StartDamageColling(float time, const std::string& attackName) {
    isDamageColling_        = true;
    lastReceivedAttackName_ = attackName;
    damageCollTime_         = time;
}

///========================================================
/// ダメージクーリングリセット
///========================================================
void EnemyBehaviorController::ResetDamageCooling() {
    isDamageColling_ = false;
    damageCollTime_  = 0.0f;
}

///========================================================
/// プール返却前クリーンアップ
///========================================================
void EnemyBehaviorController::Reset() {
    moveBehavior_   = nullptr;
    damageBehavior_ = nullptr;
    ResetDamageCooling();
    lastReceivedAttackName_.clear();
}

///========================================================
/// private
///========================================================
void EnemyBehaviorController::DamageCollingUpdate(float deltaTime) {
    if (!isDamageColling_) {
        return;
    }
    damageCollTime_ -= deltaTime;
    if (damageCollTime_ <= 0.0f) {
        isDamageColling_ = false;
    }
}

bool EnemyBehaviorController::IsChangeLocked() const {
    return pOwner_->GetIsDeath() ||
           dynamic_cast<EnemyDeath*>(damageBehavior_.get()) != nullptr;
}
