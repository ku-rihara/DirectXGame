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

    // スプリング追従パラメータ
    static constexpr float kSpringFactor    = 2.0f;   // 追従の強さ
    static constexpr float kMaxFlockSpeed   = 2.0f;   // 最大移動速度
    static constexpr float kCloseEnough     = 0.5f;   // 目標到達判定距離

    // フォーメーションスロットパラメータ
    static constexpr float kFormationRadius = 6.0f;   // ボスを中心とした円陣の半径

    bool isRunning_ = false;
};
