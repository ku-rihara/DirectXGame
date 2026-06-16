#pragma once

#include "../BaseEnemyBehavior.h"

class EntourageEnemy;

/// <summary>
/// ザコ敵がボスの周囲を群れるビヘイビア
/// </summary>
class ZakoFlockBehavior : public BaseEnemyBehavior {
public:
    ZakoFlockBehavior(EntourageEnemy* enemy);
    ~ZakoFlockBehavior() override = default;

    void Update() override;
    void Debug() override;

private:
    EntourageEnemy* pEntourageEnemy_;

    static constexpr float kCloseEnough = 0.5f; // 目標到達判定距離

    bool isRunning_ = false;
};
