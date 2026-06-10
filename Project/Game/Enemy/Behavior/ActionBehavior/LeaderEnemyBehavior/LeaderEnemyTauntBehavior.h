#pragma once

#include "LeaderEnemyBehaviorBase.h"
#include <functional>

/// <summary>
/// LeaderEnemyがプレイヤーを煽るビヘイビア
/// </summary>
class LeaderEnemyTauntBehavior : public LeaderEnemyBehaviorBase {
public:
    LeaderEnemyTauntBehavior(LeaderEnemy* enemy);
    ~LeaderEnemyTauntBehavior() override = default;

    void Update() override;
    void Debug() override;

    void SetOnFontMovingStart(std::function<void()> cb) { onFontMovingStart_ = std::move(cb); }

private:
    bool fontMovingPlayed_ = false;
    std::function<void()> onFontMovingStart_;
};