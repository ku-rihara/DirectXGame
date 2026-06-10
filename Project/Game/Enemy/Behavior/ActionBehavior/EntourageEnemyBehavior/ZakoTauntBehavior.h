#pragma once

#include "../BaseEnemyBehavior.h"

class EntourageEnemy;

/// <summary>
/// ザコ敵がプレイヤーを煽るビヘイビア
/// ボスがプレイヤーに近づいたときに発動
/// </summary>
class ZakoTauntBehavior : public BaseEnemyBehavior {
public:
    ZakoTauntBehavior(EntourageEnemy* enemy);
    ~ZakoTauntBehavior() override = default;

    void Update() override;
    void Debug() override;

private:
    EntourageEnemy* pEntourageEnemy_;
};
