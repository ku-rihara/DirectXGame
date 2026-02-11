#pragma once

#include "../BaseEnemyBehavior.h"
#include <Vector3.h>
#include <functional>

class EnemyWait : public BaseEnemyBehavior {
public:
    EnemyWait(BaseEnemy* boss);
    ~EnemyWait();

    void Update() override;
    void Debug() override;

private:
    /// <summary>
    /// 待機状態の処理
    /// </summary>
    void UpdateWaiting();

    /// <summary>
    /// 発見モーション中の処理
    /// </summary>
    void UpdateDiscovery();

    /// <summary>
    /// 終了状態の処理（次のBehaviorへ遷移）
    /// </summary>
    void UpdateEnd();

private:
    std::function<void()> currentPhase_;
    float distance_;

    // プレイヤーからの方向（逃げる方向）
    Vector3 escapeDirection_;
};
