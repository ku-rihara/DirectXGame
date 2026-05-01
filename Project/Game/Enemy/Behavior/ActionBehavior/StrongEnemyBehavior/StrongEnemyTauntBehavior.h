#pragma once

#include "StrongEnemyBehaviorBase.h"

/// <summary>
/// StrongEnemyがプレイヤーを煽るビヘイビア
/// タウント範囲外になったらEnemyChaseへ遷移
/// </summary>
class StrongEnemyTauntBehavior : public StrongEnemyBehaviorBase {
public:
    StrongEnemyTauntBehavior(StrongEnemy* enemy);
    ~StrongEnemyTauntBehavior() override = default;

    void Update() override;
    void Debug() override;
};