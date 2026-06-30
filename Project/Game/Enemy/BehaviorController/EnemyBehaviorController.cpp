#include "EnemyBehaviorController.h"
// Enemy
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/EnemyManager/EnemyManager.h"
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionController.h"
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionData.h"
// Behaviors
#include "Enemy/Behavior/ActionBehavior/BaseEnemyBehavior.h"
#include "Enemy/Behavior/ActionBehavior/CommonBehavior/EnemySpawn.h"
#include "Enemy/Behavior/DamageReactionBehavior/BaseEnemyDamageReaction.h"
#include "Enemy/Behavior/DamageReactionBehavior/EnemyDamageReactionRoot.h"
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
    animReplayTimer_         = 0.0f;
    animReplayAttackName_.clear();
    moveBehavior_   = nullptr;
    damageBehavior_ = nullptr;
}

///========================================================
/// 毎フレーム更新
///========================================================
void EnemyBehaviorController::Update(float deltaTime) {
    // ダメージリアクション中でなければ移動ビヘイビアを更新
    if (moveBehavior_) {
        bool damageActive = damageBehavior_ && !damageBehavior_->IsReactionRoot();
        if (!damageActive) {
            moveBehavior_->Update();
        }
    }

    DamageCollingUpdate(deltaTime);
    animReplayTimer_ += deltaTime;

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

    // スポーンビヘイビアをスキップして次のビヘイビアへ移行する
    if (moveBehavior_ && moveBehavior_->IsSpawn()) {
        pOwner_->ScaleReset();
        pOwner_->OnSpawnCompleted();
        pOwner_->SetIsAdaptCollision(true);
        // EnemySpawn を post-spawn ビヘイビアに差し替え
        moveBehavior_ = pOwner_->CreatePostSpawnBehavior();
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
    if (damageBehavior_ && damageBehavior_->IsDeath()) {
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
      
        auto* controller = pOwner_->GetBaseInfo()->GetManager()->GetDamageReactionController();
        if (controller) {
            const auto* reactionData = controller->GetAttackByTriggerName(attackName);
            if (reactionData && reactionData->GetReactionParam().normalParam.knockBackTime <= 0.0f) {
                const EnemyDamageRenditionData* rendition = reactionData->GetRendition();
                if (rendition) {
                    const auto& particle = rendition->GetParticleEffectParam();
                    if (!particle.fileName.empty() && particle.fileName != "None" && particle.startTiming <= 0.0f) {
                        if (auto* effects = pOwner_->GetEnemyEffects()) {
                            effects->Emit(particle.fileName);
                        }
                    }
                }
            }
        }
        return;
    }

    attackController->NotifyDamageHit();

    // 同一攻撃が animReplayMinTime 未満で再ヒットした場合はアニメーション再再生をスキップ
    bool skipAnimation = false;
    if (animReplayAttackName_ == attackName) {
        auto* controller = pOwner_->GetBaseInfo()->GetManager()->GetDamageReactionController();
        if (controller) {
            const auto* reactionData = controller->GetAttackByTriggerName(attackName);
            if (reactionData) {
                float minTime = reactionData->GetReactionParam().animReplayMinTime;
                if (minTime > 0.0f && animReplayTimer_ < minTime) {
                    skipAnimation = true;
                }
            }
        }
    }

    // アニメーションを再生する場合はタイマーをリセット
    if (!skipAnimation) {
        animReplayTimer_ = 0.0f;
    }
    animReplayAttackName_ = attackName;

    ChangeDamageReactionBehavior(std::make_unique<EnemyDamageReactionRoot>(pOwner_));

    if (damageBehavior_ && damageBehavior_->IsReactionRoot()) {
        static_cast<EnemyDamageReactionRoot*>(damageBehavior_.get())->SelectDamageActionBehaviorByAttack(attackController, skipAnimation);
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
    animReplayTimer_ = 0.0f;
    animReplayAttackName_.clear();
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
           (damageBehavior_ && damageBehavior_->IsDeath());
}
