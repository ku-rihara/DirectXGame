#include "EnemyPool.h"
#include "Enemy/EnemyManager/EnemyManager.h"
#include <cassert>

void EnemyPool::Init(EnemyManager* manager) {

    auto* param = manager->GetParam();

    // プールにEntourageEnemyのインスタンスを事前確保
    for (int32_t i = 0; i < kEntourageEnemyPoolSize; ++i) {
        auto entourageEnemy = std::make_unique<EntourageEnemy>();

        // 初期化やパラメータセットなどを行う
        entourageEnemy->GetBaseInfo()->SetManager(manager);
        entourageEnemy->GetBaseInfo()->SetParameter(BaseEnemy::Type::NORMAL, param->GetBaseParam(BaseEnemy::Type::NORMAL));
        entourageEnemy->Init(Vector3::ZeroVector());
        entourageEnemy->PrepareForPool();

        // 取り巻き敵プールに追加
        entourageEnemyPool_.Release(std::move(entourageEnemy));
    }

    // プールにLeaderEnemyのインスタンスを事前確保
    for (int32_t i = 0; i < kLeaderEnemyPoolSize; ++i) {
        auto leaderEnemy = std::make_unique<LeaderEnemy>();

        // 初期化やパラメータセットなどを行う
        leaderEnemy->GetBaseInfo()->SetManager(manager);
        leaderEnemy->GetBaseInfo()->SetParameter(BaseEnemy::Type::STRONG, param->GetBaseParam(BaseEnemy::Type::STRONG));
        leaderEnemy->SetStrongParameter(param->GetStrongParam());
        leaderEnemy->Init(Vector3::ZeroVector());
        leaderEnemy->PrepareForPool();

        // リーダー敵プールに追加
        leaderEnemyPool_.Release(std::move(leaderEnemy));
    }
}

std::unique_ptr<BaseEnemy> EnemyPool::Acquire(BaseEnemy::Type type) {

    // タイプに応じてEntourageEnemyかLeaderEnemyをプールから取得

    // Normalタイプの敵を取得
    if (type == BaseEnemy::Type::NORMAL) {
        auto entourageEnemy = entourageEnemyPool_.Acquire();
        assert(entourageEnemy && "EntourageEnemy pool exhausted");
        if (!entourageEnemy) {
            return std::make_unique<EntourageEnemy>();
        }
        return entourageEnemy;
    } else {
        // Leaderタイプの敵を取得
        auto leaderEnemy = leaderEnemyPool_.Acquire();
        assert(leaderEnemy && "LeaderEnemy pool exhausted");
        if (!leaderEnemy) {
            return std::make_unique<LeaderEnemy>();
        }
        return leaderEnemy;
    }
}

void EnemyPool::Release(std::unique_ptr<BaseEnemy> enemy) {

    // 敵のタイプに応じて適切なプールに返却
    if (enemy->GetBaseInfo()->GetType() == BaseEnemy::Type::NORMAL) {
        entourageEnemyPool_.Release(std::unique_ptr<EntourageEnemy>(static_cast<EntourageEnemy*>(enemy.release())));
    } else {
        leaderEnemyPool_.Release(std::unique_ptr<LeaderEnemy>(static_cast<LeaderEnemy*>(enemy.release())));
    }
}
