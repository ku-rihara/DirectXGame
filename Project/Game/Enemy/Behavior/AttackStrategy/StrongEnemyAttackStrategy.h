#pragma once
#include "IEnemyAttackStrategy.h"

class StrongEnemy;

/// <summary>
/// 強敵の攻撃戦略
/// </summary>
class StrongEnemyAttackStrategy : public IEnemyAttackStrategy {
public:
    StrongEnemyAttackStrategy(BaseEnemy* enemy);
    ~StrongEnemyAttackStrategy() override = default;

    void Anticipation() override;
    bool IsAnticipationFinished() const override;
    void Start() override;
    void Update() override;
    bool IsFinished() const override;
    void Finish() override;
};
