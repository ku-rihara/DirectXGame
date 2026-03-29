#pragma once

#include "BaseEnemyDamageReaction.h"

class EnemyDeath : public BaseEnemyDamageReaction {
private:
    enum class Step {
        DIRECTIONSET,
        ANIMATION,
        BURST,
        DEATH,
    };

private:
    Step step_;
    bool deathAnimStarted_  = false;
    bool deathAnimFinished_ = false;

public:
    EnemyDeath(BaseEnemy* boss);
    ~EnemyDeath();
    void AngleCaluclation();

    void Update(float deltaTime) override;
    void Debug() override;
};