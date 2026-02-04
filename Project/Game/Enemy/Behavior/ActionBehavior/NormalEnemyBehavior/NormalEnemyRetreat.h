#pragma once

#include "NormalEnemyBehaviorBase.h"
#include "Easing/Easing.h"

class NormalEnemy;

/// <summary>
/// NormalEnemy専用の後退ビヘイビア
/// </summary>
class NormalEnemyRetreat : public NormalEnemyBehaviorBase {
public:
    NormalEnemyRetreat(NormalEnemy* enemy);
    ~NormalEnemyRetreat();

    void Update() override;
    void Debug() override;

private:
    /// <summary>
    /// プレイヤーとの距離が攻撃可能範囲内かチェック
    /// </summary>
    bool IsInAttackRange(float distance) const;

private:
    float distance_;
    bool isRetreatStarted_;

    // 攻撃前待機
    float preAttackWaitTimer_;
    bool isWaitingForPreAttack_;

    // PreDash状態管理
    bool isPreDashStarted_;

    // スケールイージング
    KetaEngine::Easing<Vector3> scaleEasing_;
    Vector3 tempEnemyScale_;
};
