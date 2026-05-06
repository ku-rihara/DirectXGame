#pragma once

#include "StrongEnemyBehaviorBase.h"

class StrongEnemy;

/// <summary>
/// StrongEnemyがプレイヤーから遠ざかるビヘイビア
/// 通常はプレイヤーと反対方向へ走り、境界(fleeDistance以内)に近づいたら中心方向へ補正する。
/// 一度境界に近づいた後、fleeDistance以上離れたらStartTaunt()へ遷移する。
/// </summary>
class StrongEnemyFleeBehavior : public StrongEnemyBehaviorBase {
public:
    /// @param startDelay 0より大きいと走り出す前にその秒数だけ待機する
    explicit StrongEnemyFleeBehavior(StrongEnemy* enemy, float startDelay = 0.0f);
    ~StrongEnemyFleeBehavior() override = default;

    void Update() override;
    void Debug() override;

private:
    float startDelayTimer_;
};
