#pragma once

#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/NormalEnemy.h"
#include "Enemy/Types/StrongEnemy.h"
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
    const int32_t kNormalEnemyPoolSize = 48;
    const int32_t kStrongEnemyPoolSize = 8;

    // NormalEnemyとStrongEnemyのオブジェクトプール
    ObjectPool<NormalEnemy> normalPool_;
    ObjectPool<StrongEnemy> strongPool_;
};
