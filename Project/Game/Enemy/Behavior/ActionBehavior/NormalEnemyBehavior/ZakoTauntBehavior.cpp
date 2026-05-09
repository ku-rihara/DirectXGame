#include "ZakoTauntBehavior.h"

#include "Enemy/Effects/EnemyEffects.h"
#include "Enemy/Types/NormalEnemy.h"
#include "Enemy/Types/BaseEnemy.h"

ZakoTauntBehavior::ZakoTauntBehavior(NormalEnemy* enemy)
    : BaseEnemyBehavior("ZakoTaunt", static_cast<BaseEnemy*>(enemy))
    , pNormalEnemy_(enemy) {

    // 煽りアニメーション再生
    pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Taunt, true);
    // 煽りエフェクト開始
    pBaseEnemy_->GetEnemyEffects()->Emit("TauntZako");
}

void ZakoTauntBehavior::Update() {
    // プレイヤーの方向を向く
    pBaseEnemy_->DirectionToPlayer();

    // ゴール位置をボスの現在位置に更新
    BaseEnemy* boss = pNormalEnemy_->GetBoss();
    if (boss && !boss->GetIsDeath()) {
        pBaseEnemy_->GetEnemyEffects()->SetTargetObjectPos(boss->GetWorldPosition());
    }

    // 煽りエフェクト継続
    pBaseEnemy_->GetEnemyEffects()->Emit("TauntZako");
}

void ZakoTauntBehavior::Debug() {
}
