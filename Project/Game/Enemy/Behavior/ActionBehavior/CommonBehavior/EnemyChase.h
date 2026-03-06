#pragma once

#include "../BaseEnemyBehavior.h"

/// <summary>
/// 敵の追跡ビヘイビア
/// </summary>
class EnemyChase : public BaseEnemyBehavior {
public:
    EnemyChase(BaseEnemy* enemy);
    ~EnemyChase();

    void Update() override;
    void Debug() override;

private:
    // 追跡タイマー
    float chaseTimer_;
};
