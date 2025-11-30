/// behavior
#include "EnemyDamageReactionRoot.h"
#include "EnemyDamageReacrtionAction.h"
/// obj
#include "CollisionBox/PlayerCollisionInfo.h"
#include "Enemy/EnemyManager.h"
#include "Enemy/Types/BaseEnemy.h"
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

void EnemyDamageReactionRoot::Update() {
    // 待機
}

void EnemyDamageReactionRoot::Debug() {
}

void EnemyDamageReactionRoot::SelectDamageActionBehaviorByAttack(const PlayerCollisionInfo* playerCollisionInfo) {
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
        // 対応するリアクションデータが見つからない場合は何もしない
        return;
    }

    // ダメージクールタイムの開始
    pBaseEnemy_->StartDamageColling(reactionData->GetReactionParam().damageCollingTime, attackName);

    // ダメージを適用
    if (pPlayerCollisionInfo_) {
        pBaseEnemy_->TakeDamage(pPlayerCollisionInfo_->GetAttackPower());

        // リアクションアクションに切り替え
        pBaseEnemy_->ChangeDamageReactionBehavior(
            std::make_unique<EnemyDamageReactionAction>(pBaseEnemy_, reactionData, pPlayerCollisionInfo_));
    }
}