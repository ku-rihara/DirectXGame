#include "LeaderEnemyTauntBehavior.h"

#include "Enemy/Types/LeaderEnemy.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"

LeaderEnemyTauntBehavior::LeaderEnemyTauntBehavior(LeaderEnemy* enemy)
    : LeaderEnemyBehaviorBase("LeaderEnemyTauntBehavior", enemy) {

    pBaseEnemy_->GetAnimator()->PlayAnimation(BaseEnemy::AnimationType::Taunt, true);
}


void LeaderEnemyTauntBehavior::Update() {
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

void LeaderEnemyTauntBehavior::Debug() {
}