#pragma once

#include "../BaseEnemyBehavior.h"
#include "Vector3.h"

class NormalEnemy;

/// <summary>
/// ザコ敵がボスの周囲を群れるビヘイビア
/// </summary>
class ZakoFlockBehavior : public BaseEnemyBehavior {
public:
    ZakoFlockBehavior(NormalEnemy* enemy);
    ~ZakoFlockBehavior() override = default;

    void Update() override;
    void Debug() override;

private:
    NormalEnemy* pNormalEnemy_;

    static constexpr float kCloseEnough = 0.5f; // 目標到達判定距離

    bool isRunning_ = false;
};
