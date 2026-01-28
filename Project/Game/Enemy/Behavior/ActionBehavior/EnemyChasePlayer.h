#pragma once

#include "BaseEnemyBehavior.h"
#include "Easing/Easing.h"

class EnemyChasePlayer : public BaseEnemyBehavior {
private:
    float chaseSpeedMax_;
    float chaseSpeedNormal_;

    float distance_;

    bool isChase_;

    // 追従時間管理
    float currentChaseTime_; // 現在の追従時間
    float chaseResetTimer_; // 追従リセットタイマー
    bool isChaseTimeOver_; // 追従時間超過フラグ

    KetaEngine::Easing<Vector3> scaleEasing_;
    Vector3 tempEnemyScale_;

public:
    // コンストラクタ
    EnemyChasePlayer(BaseEnemy* boss);
    ~EnemyChasePlayer();

    void Update() override;
    void Debug() override;

private:
    /// <summary>
    /// 他の敵との回避ベクトルを計算
    /// </summary>
    Vector3 CalculateAvoidanceVector();

    /// <summary>
    /// 追従時間を更新
    /// </summary>
    void UpdateChaseTime(float deltaTime);
};