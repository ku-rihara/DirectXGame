#pragma once
#include "IEnemyAttackStrategy.h"

class NormalEnemy;

/// <summary>
/// 通常敵の攻撃戦略
/// </summary>
class NormalEnemyAttackStrategy : public IEnemyAttackStrategy {
public:
    NormalEnemyAttackStrategy(BaseEnemy* enemy);
    ~NormalEnemyAttackStrategy() override = default;

    void Anticipation() override;
    bool IsAnticipationFinished() const override;
    void Start() override;
    void Update() override;
    bool IsFinished() const override;
    void Finish() override;
};
