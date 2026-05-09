#pragma once

#include "StrongEnemyBehaviorBase.h"
#include <functional>

/// <summary>
/// StrongEnemyがプレイヤーを煽るビヘイビア
/// </summary>
class StrongEnemyTauntBehavior : public StrongEnemyBehaviorBase {
public:
    StrongEnemyTauntBehavior(StrongEnemy* enemy);
    ~StrongEnemyTauntBehavior() override = default;

    void Update() override;
    void Debug() override;

    void SetOnFontMovingStart(std::function<void()> cb) { onFontMovingStart_ = std::move(cb); }

private:
    bool fontMovingPlayed_ = false;
    std::function<void()> onFontMovingStart_;
};