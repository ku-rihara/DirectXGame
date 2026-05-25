#include "StrongEnemyTauntBehavior.h"

#include "Enemy/Types/StrongEnemy.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"

StrongEnemyTauntBehavior::StrongEnemyTauntBehavior(StrongEnemy* enemy)
    : StrongEnemyBehaviorBase("StrongEnemyTauntBehavior", enemy) {

    pBaseEnemy_->GetAnimator()->PlayAnimation(BaseEnemy::AnimationType::Taunt, true);
}


void StrongEnemyTauntBehavior::Update() {
    pBaseEnemy_->DirectionToPlayer();

    // SpawnTauntFont終了後にTauntFontMovingへ切り替え
    if (!fontMovingPlayed_) {
        if (auto* font = GetEnemy()->GetTauntFont()) {
            auto* player = font->transform_.GetObjEaseAnimationPlayer();
            if (player && player->IsFinished()) {
                if (onFontMovingStart_) {
                    onFontMovingStart_();
                }
                fontMovingPlayed_ = true;
            }
        }
    }

 
}

void StrongEnemyTauntBehavior::Debug() {
}