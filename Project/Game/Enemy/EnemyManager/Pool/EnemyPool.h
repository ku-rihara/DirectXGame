#pragma once

// Enemy
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/EntourageEnemy.h"
#include "Enemy/Types/LeaderEnemy.h"
// ObjectPool
#include "ObjectPool/ObjectPool.h"
// std
#include <memory>

class EnemyManager;

/// <summary>
/// 敵のオブジェクトプールを管理するクラス
/// </summary>
class EnemyPool {
public:
    EnemyPool()  = default;
    ~EnemyPool() = default;

    /// <summary>
    /// プールを事前確保する
    /// </summary>
    void Init(EnemyManager* manager);

    // プールから1体取得  
    std::unique_ptr<BaseEnemy> Acquire(BaseEnemy::Type type);
    // プールへ返却
    void Release(std::unique_ptr<BaseEnemy> enemy);

private:

    // 各敵種類のプールサイズ
    const int32_t kEntourageEnemyPoolSize = 48;
    const int32_t kLeaderEnemyPoolSize = 8;

    // EntourageEnemyとLeaderEnemyのオブジェクトプール
    ObjectPool<EntourageEnemy> entourageEnemyPool_;
    ObjectPool<LeaderEnemy> leaderEnemyPool_;
};
