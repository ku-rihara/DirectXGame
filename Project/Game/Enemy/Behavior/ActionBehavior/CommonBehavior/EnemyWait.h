#pragma once

#include "../BaseEnemyBehavior.h"
#include "Easing/Easing.h"
#include <Vector3.h>

class EnemyWait : public BaseEnemyBehavior {
public:
    // コンストラクタ
    EnemyWait(BaseEnemy* boss);
    ~EnemyWait();

    void Update() override;
    void Debug() override;

private:
    /// <summary>
    /// 攻撃後待機時間を更新
    /// </summary>
    void UpdateAttackCooldown();

private:
    float distance_;

    // 攻撃後待機時間
    float moveCollTimer_;
    bool isMoveCollTime_;

    // プレイヤー発見後の遅延
    bool hasDiscoveredPlayer_;
    bool isPreDashAnimFinished_;
    float discoveryDelayTimer_;

    Vector3 directionToPlayer_;
    float objectiveAngle_;
};
