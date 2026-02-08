#pragma once

#include "../BaseEnemyBehavior.h"
#include <Vector3.h>

/// <summary>
/// 敵の逃走ビヘイビア
/// </summary>
class EnemyEscape : public BaseEnemyBehavior {
public:
    EnemyEscape(BaseEnemy* enemy, const Vector3& escapeDirection);
    ~EnemyEscape();

    void Update() override;
    void Debug() override;

private:
    // 逃走タイマー
    float escapeTimer_;

    // 逃げる方向
    Vector3 escapeDirection_;

    // 目標角度
    float targetAngle_;
};
