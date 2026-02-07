/// behavior
#include "EnemyDamageReactionRoot.h"
#include "EnemyDamageReactionAction.h"
#include "EnemyDamageReactionNormal.h"
#include "EnemyDamageReactionSlammed.h"
#include "EnemyDamageReactionTakeUpper.h"
#include "EnemyDeath.h"
/// Enemy
#include "Enemy/EnemyManager.h"
#include "Enemy/Types/BaseEnemy.h"
// Field
#include "Field/SideRope/SideRope.h"
// Player
#include "Player/CollisionBox/PlayerAttackCollisionBox.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"
/// data
#include "Enemy/DamageReaction/EnemyDamageReactionController.h"
#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
/// math
#include "Frame/Frame.h"
#include "MathFunction.h"

// 初期化
EnemyDamageReactionRoot::EnemyDamageReactionRoot(BaseEnemy* boss)
    : BaseEnemyDamageReaction("EnemyDamageReactionRoot", boss) {

    pReactionController_ = pBaseEnemy_->GetManager()->GetDamageReactionController();
}

EnemyDamageReactionRoot::~EnemyDamageReactionRoot() {
}

void EnemyDamageReactionRoot::Update(float deltaTime) {
    // 待機
    deltaTime;
}

void EnemyDamageReactionRoot::Debug() {
}

void EnemyDamageReactionRoot::SelectDamageActionBehaviorByAttack(const PlayerAttackCollisionBox* playerCollisionInfo) {
    if (!playerCollisionInfo) {
        return;
    }

    if (!playerCollisionInfo->GetComboAttackData()) {
        return;
    }

    pPlayerCollisionInfo_ = playerCollisionInfo;
    // プレイヤーの攻撃名を取得
    std::string attackName = playerCollisionInfo->GetComboAttackData()->GetGroupName();

    // 攻撃名が空でない場合、リアクションを適用
    if (!attackName.empty()) {
        ApplyReactionByAttackName(attackName);
    }
}

void EnemyDamageReactionRoot::ApplyReactionByAttackName(const std::string& attackName) {
    if (!pReactionController_) {
        return;
    }

    // 攻撃名に対応するリアクションデータを検索
    EnemyDamageReactionData* reactionData = pReactionController_->GetAttackByTriggerName(attackName);

    if (!reactionData) {
        return;
    }

    // ダメージクールタイムの開始
    pBaseEnemy_->StartDamageColling(reactionData->GetReactionParam().damageCollingTime, attackName);

    // ダメージを適用
    if (!pPlayerCollisionInfo_) {
        return;
    }

    // ダメージを受ける
    pBaseEnemy_->TakeDamage(pPlayerCollisionInfo_->GetAttackPower());

    if (pBaseEnemy_->GetHP() <= 0.0f) {
        // 死亡リアクションに変更
        ChangeDeathReaction(reactionData);
    } else {
        // リアクション状態に応じたBehaviorに切り替え
        int reactionState = reactionData->GetReactionParam().intReactionState;

        if (reactionState == static_cast<int>(EnemyDamageReactionAction::ReactionState::Normal)) {
            pBaseEnemy_->ChangeDamageReactionBehavior(
                std::make_unique<EnemyDamageReactionNormal>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_));
        } else if (reactionState == static_cast<int>(EnemyDamageReactionAction::ReactionState::Slammed)) {
            pBaseEnemy_->ChangeDamageReactionBehavior(
                std::make_unique<EnemyDamageReactionSlammed>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_));
        } else if (reactionState == static_cast<int>(EnemyDamageReactionAction::ReactionState::TakeUpper)) {
            pBaseEnemy_->ChangeDamageReactionBehavior(
                std::make_unique<EnemyDamageReactionTakeUpper>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_));
        }
    }
}

void EnemyDamageReactionRoot::ChangeDeathReaction(EnemyDamageReactionData* reactionData) {
    // 死亡予約フラグを立てる
    pBaseEnemy_->SetIsDeathPending(true);

    // コリジョンを無効化
    pBaseEnemy_->SetIsAdaptCollision(false);

    // ダメージパーティクルエフェクトを再生
    PlayDamageParticleEffect(reactionData);

    int reactionState = reactionData->GetReactionParam().intReactionState;

    // Slammed状態の場合は、Slammedリアクションに切り替え
    if (reactionState == static_cast<int>(EnemyDamageReactionAction::ReactionState::Slammed)) {
        pBaseEnemy_->ChangeDamageReactionBehavior(
            std::make_unique<EnemyDamageReactionSlammed>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_));
    }
    // TakeUpper状態の場合は、TakeUpperリアクションに切り替え
    else if (reactionState == static_cast<int>(EnemyDamageReactionAction::ReactionState::TakeUpper)) {
        pBaseEnemy_->ChangeDamageReactionBehavior(
            std::make_unique<EnemyDamageReactionTakeUpper>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_));
    }
    // それ以外の場合は即座に死亡Behaviorに切り替え
    else {
        pBaseEnemy_->ChangeDamageReactionBehavior(
            std::make_unique<EnemyDeath>(pBaseEnemy_));
    }
}

void EnemyDamageReactionRoot::PlayDamageParticleEffect(EnemyDamageReactionData* reactionData) {
    if (!reactionData) {
        return;
    }

    // 演出データの取得
    const EnemyDamageRenditionData* rendition = reactionData->GetRendition();
    if (!rendition) {
        return;
    }

    // パーティクルエフェクトの再生
    const auto& particleParam = rendition->GetParticleEffectParam();
    if (!particleParam.fileName.empty() && particleParam.fileName != "None") {
        if (pBaseEnemy_->GetEnemyEffects()) {
            pBaseEnemy_->GetEnemyEffects()->Emit(particleParam.fileName);
        }
    }
}