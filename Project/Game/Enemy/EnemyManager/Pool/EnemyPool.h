#pragma once

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/EntourageEnemy.h"
#include "Enemy/Types/LeaderEnemy.h"
#include "ObjectPool/ObjectPool.h"

#include <memory>

/// <summary>
/// 敵のオブジェクトプールを管理するクラス
/// </summary>
class EnemyPool {
public:
    EnemyPool()  = default;
    ~EnemyPool() = default;

    void Init();

    std::unique_ptr<BaseEnemy> Acquire(BaseEnemy::Type type);
    void Release(std::unique_ptr<BaseEnemy> enemy);

private:

    // プールサイズ
    const int32_t kEntourageEnemyPoolSize = 48;
    const int32_t kLeaderEnemyPoolSize = 8;

    // EntourageEnemyとLeaderEnemyのオブジェクトプール
    ObjectPool<EntourageEnemy> normalPool_;
    ObjectPool<LeaderEnemy> strongPool_;
};
