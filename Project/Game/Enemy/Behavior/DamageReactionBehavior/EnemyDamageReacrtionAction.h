#pragma once

#include "BaseEnemyDamageReaction.h"

class EnemyDamageReactionData;
class EnemyDamageReactionAction : public BaseEnemyDamageReaction {
public:
    // コンストラクタ
    EnemyDamageReactionAction(BaseEnemy* boss, EnemyDamageReactionData* reactionData);
    ~EnemyDamageReactionAction() override;

    void Update() override;
    void Debug() override;

private:

    EnemyDamageReactionData* pReactionData_ = nullptr;
};
