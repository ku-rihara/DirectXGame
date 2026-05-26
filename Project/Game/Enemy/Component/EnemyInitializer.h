#pragma once
#include "Vector3.h"

class BaseEnemy;

/// <summary>
/// BaseEnemy::Init の処理を担うクラス。
/// </summary>
class EnemyInitializer {
public:
    EnemyInitializer(BaseEnemy* enemy, const Vector3& spawnPos);

    void Run();

private:

    // 各初期化処理
    void ResetFlags();
    void InitAnimator();
    void InitBehaviorController();
    void InitHP();
    void InitUI();
    void InitTransform();
    void InitAttackCollision();
    void InitEffects();
    void InitDamageReaction();
    void ApplyCollision();

private:
    BaseEnemy* enemy_;
    Vector3    spawnPos_;
};
