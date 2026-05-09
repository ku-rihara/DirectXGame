#include "ZakoTauntBehavior.h"

#include "Enemy/Types/NormalEnemy.h"
#include "Enemy/Types/BaseEnemy.h"

ZakoTauntBehavior::ZakoTauntBehavior(NormalEnemy* enemy)
    : BaseEnemyBehavior("ZakoTaunt", static_cast<BaseEnemy*>(enemy))
    , pNormalEnemy_(enemy) {

    // 煽りアニメーション再生
    pBaseEnemy_->PlayAnimation(BaseEnemy::AnimationType::Taunt, true);
}

void ZakoTauntBehavior::Update() {
    // プレイヤーの方向を向く
    pBaseEnemy_->DirectionToPlayer();
}

void ZakoTauntBehavior::Debug() {
}
