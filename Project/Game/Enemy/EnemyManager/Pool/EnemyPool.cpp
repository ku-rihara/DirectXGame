#include "EnemyPool.h"
#include <cassert>

void EnemyPool::Init() {

    // プールにEntourageEnemyのインスタンスを事前確保
    for (int32_t i = 0; i < kEntourageEnemyPoolSize; ++i) {
        auto entourageEnemy = std::make_unique<EntourageEnemy>();
        entourageEnemy->SetIsAdaptCollision(false);
        normalPool_.Release(std::move(entourageEnemy));
    }

    // プールにLeaderEnemyのインスタンスを事前確保
    for (int32_t i = 0; i < kLeaderEnemyPoolSize; ++i) {
        auto leaderEnemy = std::make_unique<LeaderEnemy>();
        leaderEnemy->SetIsAdaptCollision(false);
        strongPool_.Release(std::move(leaderEnemy));
    }
}

std::unique_ptr<BaseEnemy> EnemyPool::Acquire(BaseEnemy::Type type) {

    // タイプに応じてEntourageEnemyかLeaderEnemyをプールから取得
    if (type == BaseEnemy::Type::NORMAL) {
        auto entourageEnemy = normalPool_.Acquire();
        assert(entourageEnemy && "EntourageEnemy pool exhausted");
        if (!entourageEnemy) {
            return std::make_unique<EntourageEnemy>();
        }
        return entourageEnemy;
    } else {
        auto leaderEnemy = strongPool_.Acquire();
        assert(leaderEnemy && "LeaderEnemy pool exhausted");
        if (!leaderEnemy) {
            return std::make_unique<LeaderEnemy>();
        }
        return leaderEnemy;
    }
}

void EnemyPool::Release(std::unique_ptr<BaseEnemy> enemy) {
    if (enemy->GetBaseInfo()->GetType() == BaseEnemy::Type::NORMAL) {
        normalPool_.Release(std::unique_ptr<EntourageEnemy>(static_cast<EntourageEnemy*>(enemy.release())));
    } else {
        strongPool_.Release(std::unique_ptr<LeaderEnemy>(static_cast<LeaderEnemy*>(enemy.release())));
    }
}
