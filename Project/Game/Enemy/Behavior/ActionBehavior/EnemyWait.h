#pragma once

#include "BaseEnemyBehavior.h"
#include "Easing/Easing.h"

class EnemyWait : public BaseEnemyBehavior {
public:
    // コンストラクタ
    // skipDiscoveryDelay: trueの場合、発見遅延をスキップして即座に追跡可能
    EnemyWait(BaseEnemy* boss, bool skipDiscoveryDelay = false);
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
    bool isChase_;

    // 攻撃後待機時間
    float attackCooldownTimer_;
    bool isInAttackCooldown_;

    // プレイヤー発見後の遅延
    bool hasDiscoveredPlayer_;
    bool isPreDashAnimFinished_;  // PreDashアニメーション終了フラグ
    float discoveryDelayTimer_;   // アニメーション終了後の遅延タイマー

    KetaEngine::Easing<Vector2> spriteEase_;
    Vector2 tempSpriteScale_;
};
