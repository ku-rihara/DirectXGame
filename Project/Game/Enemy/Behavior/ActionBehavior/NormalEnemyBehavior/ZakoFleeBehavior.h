#pragma once

#include "../BaseEnemyBehavior.h"

class NormalEnemy;

/// <summary>
/// ボスが倒されたときにザコ敵がビビって逃げるビヘイビア
/// </summary>
class ZakoFleeBehavior : public BaseEnemyBehavior {
public:
    ZakoFleeBehavior(NormalEnemy* enemy);
    ~ZakoFleeBehavior() override = default;

    void Update() override;
    void Debug() override;

private:
    NormalEnemy* pNormalEnemy_;

    static constexpr float kMaxFleeTime  = 6.0f;   // 最大逃走時間（秒）
    static constexpr float kSpeedRampUp  = 0.6f;   // この秒数かけて最高速に到達

    float fleeTimer_ = 0.0f;
};
