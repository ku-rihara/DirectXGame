/// behavior
#include "EnemyDamageReactionRoot.h"
#include "EnemyDamageReactionNormal.h"
#include "EnemyDamageReactionSlammed.h"
#include "EnemyDamageReactionTakeUpper.h"
#include "EnemyDeath.h"
/// Enemy
#include "Enemy/EnemyManager/EnemyManager.h"
#include "Enemy/Types/BaseEnemy.h"
// Field
#include "Field/SideRope/SideRope.h"
// Player
#include "Player/Components/CollisionBox/PlayerAttackCollider.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"
/// data
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionController.h"
#include "Enemy/EnemyManager/DamageReaction/EnemyDamageReactionData.h"
/// math

EnemyDamageReactionRoot::EnemyDamageReactionRoot(BaseEnemy* boss)
    : BaseEnemyDamageReaction("EnemyDamageReactionRoot", boss) {

    pReactionController_ = pBaseEnemy_->GetBaseInfo()->GetManager()->GetDamageReactionController();
}

EnemyDamageReactionRoot::~EnemyDamageReactionRoot() {
}

void EnemyDamageReactionRoot::Update(float deltaTime) {
    deltaTime;

    // 死亡予約済みでRootに戻った場合のフォールバック
    if (pBaseEnemy_->GetIsDeathPending() && !pBaseEnemy_->GetIsDeath() && pBaseEnemy_->GetHP() <= 0.0f) {
        pBaseEnemy_->SetIsAdaptCollision(false);
        pBaseEnemy_->ChangeDamageReactionBehavior(std::make_unique<EnemyDeath>(pBaseEnemy_));
    }
}

void EnemyDamageReactionRoot::Debug() {
}

void EnemyDamageReactionRoot::SelectDamageActionBehaviorByAttack(const PlayerAttackCollider* playerCollisionInfo, bool skipAnimation) {
    // null チェック
    if (!playerCollisionInfo) {
        return;
    }

    if (!playerCollisionInfo->GetComboAttackData()) {
        return;
    }

    // ポインタ保存
    pPlayerCollisionInfo_ = playerCollisionInfo;
    skipAnimation_        = skipAnimation;

    // 攻撃名でリアクションを選択
    std::string attackName = playerCollisionInfo->GetComboAttackData()->GetGroupName();

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

    if (!pPlayerCollisionInfo_) {
        return;
    }

    if (!reactionData) {
        // 設定されていない攻撃: デフォルトアニメーション + ノックバックのみ
        int enemyType = static_cast<int>(pBaseEnemy_->GetBaseInfo()->GetType());
        pBaseEnemy_->StartDamageColling(pReactionController_->GetDefaultDamageCoolTime(enemyType), attackName);
        pBaseEnemy_->TakeDamage(pPlayerCollisionInfo_->GetAttackPower());

        // HP に応じて死亡 or ノックバックへ遷移
        if (pBaseEnemy_->GetHP() <= 0.0f) {
            ChangeDeathReaction();
        } else {
            pBaseEnemy_->ChangeDamageReactionBehavior(
                std::make_unique<EnemyDamageReactionNormal>(pBaseEnemy_, nullptr, pPlayerCollisionInfo_, skipAnimation_));
        }
        return;
    }

    // ダメージクールタイムの開始
    pBaseEnemy_->StartDamageColling(reactionData->GetReactionParam().damageCollingTime, attackName);

    // ダメージを受ける
    pBaseEnemy_->TakeDamage(pPlayerCollisionInfo_->GetAttackPower());

    if (pBaseEnemy_->GetHP() <= 0.0f) {
        // 死亡リアクションに変更
        ChangeDeathReaction();
    } else {
        // blowYPowerでリアクション種別を自動判定
        float blowYPower = pPlayerCollisionInfo_->GetComboAttackData()->GetAttackParam().blowYPower;

        if (blowYPower > 0.0f) {
            pBaseEnemy_->ChangeDamageReactionBehavior(
                std::make_unique<EnemyDamageReactionTakeUpper>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_));
        } else if (blowYPower < 0.0f) {
            pBaseEnemy_->ChangeDamageReactionBehavior(
                std::make_unique<EnemyDamageReactionSlammed>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_));
        } else {
            pBaseEnemy_->ChangeDamageReactionBehavior(
                std::make_unique<EnemyDamageReactionNormal>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_, skipAnimation_));
        }
    }
}

void EnemyDamageReactionRoot::ChangeDeathReaction() {
    // コリジョンを無効化
    pBaseEnemy_->SetIsAdaptCollision(false);
    pBaseEnemy_->SetIsDeathPending(true);
    pBaseEnemy_->ChangeDamageReactionBehavior(std::make_unique<EnemyDeath>(pBaseEnemy_));
}
