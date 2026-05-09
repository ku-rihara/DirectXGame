#pragma once

#include "../BaseEnemyBehavior.h"

class NormalEnemy;

/// <summary>
/// ザコ敵がプレイヤーを煽るビヘイビア
/// ボスがプレイヤーに近づいたときに発動
/// </summary>
class ZakoTauntBehavior : public BaseEnemyBehavior {
public:
    ZakoTauntBehavior(NormalEnemy* enemy);
    ~ZakoTauntBehavior() override = default;

    void Update() override;
    void Debug() override;

private:
    NormalEnemy* pNormalEnemy_;
};
