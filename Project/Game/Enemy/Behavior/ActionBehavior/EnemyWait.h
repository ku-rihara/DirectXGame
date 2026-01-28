#pragma once

#include "BaseEnemyBehavior.h"
#include "Easing/Easing.h"

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
    bool isChase_;

    // 攻撃後待機時間
    float attackCooldownTimer_;
    bool isInAttackCooldown_;

    KetaEngine::Easing<Vector2> spriteEase_;
    Vector2 tempSpriteScale_;
};
