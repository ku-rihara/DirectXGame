#pragma once

#include "LeaderEnemyBehaviorBase.h"
#include "Vector3.h"

class LeaderEnemy;

/// <summary>
/// LeaderEnemyがプレイヤーから遠ざかるビヘイビア
/// </summary>
class LeaderEnemyFleeBehavior : public LeaderEnemyBehaviorBase {
public:
    ///  0より大きいと走り出す前にその秒数だけ待機する
    explicit LeaderEnemyFleeBehavior(LeaderEnemy* enemy, float startDelay = 0.0f);
    ~LeaderEnemyFleeBehavior() override = default;

    void Update() override;
    void Debug() override;

private:
    float startDelayTimer_;
    float dirLockTimer_    = 0.0f; ///< 方向転換後のロック残り時間
    Vector3 lockedMoveDir_ = {};   ///< ロック中の移動方向
};