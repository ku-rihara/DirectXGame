#pragma once

#include "BaseEnemyDamageReaction.h"

class EnemyDeath : public BaseEnemyDamageReaction {
private:
    enum class Step {
        DIRECTIONSET,
        BLOW,
        BURST,
        DEATH,
    };

private:
    Step step_;
    float rotate_;
    float burstTime_;
    Vector3 blowPower_;
    float blowJumpValue_;
    float gravity_;
    float kFallSpeedLimit_;

public:
    // コンストラクタ
    EnemyDeath(BaseEnemy* boss);
    ~EnemyDeath();
   void  AngleCaluclation();

    void Update(float deltaTime) override;
    void Debug() override;
};