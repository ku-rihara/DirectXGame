#pragma once
#include "IEnemyAttackStrategy.h"
#include "Vector3.h"

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
    void Reset() override;

private:
    // 攻撃時の移動
    Vector3 attackDirection_;      // 攻撃方向
    float currentMoveDistance_;    // 現在の移動距離
    float targetMoveDistance_;     // 目標移動距離
};
