#include "EnemyPool.h"
#include <cassert>

void EnemyPool::Init() {

    // プールにNormalEnemyのインスタンスを事前確保
    for (int32_t i = 0; i < kNormalEnemyPoolSize; ++i) {
        auto normalEnemy = std::make_unique<NormalEnemy>();
        normalEnemy->SetIsAdaptCollision(false);
        normalPool_.Release(std::move(normalEnemy));
    }

    // プールにStrongEnemyのインスタンスを事前確保
    for (int32_t i = 0; i < kStrongEnemyPoolSize; ++i) {
        auto strongEnemy = std::make_unique<StrongEnemy>();
        strongEnemy->SetIsAdaptCollision(false);
        strongPool_.Release(std::move(strongEnemy));
    }
}

std::unique_ptr<BaseEnemy> EnemyPool::Acquire(BaseEnemy::Type type) {

    // タイプに応じてNormalEnemyかStrongEnemyをプールから取得
    if (type == BaseEnemy::Type::NORMAL) {
        auto normalEnemy = normalPool_.Acquire();
        assert(normalEnemy && "NormalEnemy pool exhausted");
        if (!normalEnemy) {
            return std::make_unique<NormalEnemy>();
        }
        return normalEnemy;
    } else {
        auto strongEnemy = strongPool_.Acquire();
        assert(strongEnemy && "StrongEnemy pool exhausted");
        if (!strongEnemy) {
            return std::make_unique<StrongEnemy>();
        }
        return strongEnemy;
    }
}

void EnemyPool::Release(std::unique_ptr<BaseEnemy> enemy) {
    if (enemy->GetBaseInfo()->GetType() == BaseEnemy::Type::NORMAL) {
        normalPool_.Release(std::unique_ptr<NormalEnemy>(static_cast<NormalEnemy*>(enemy.release())));
    } else {
        strongPool_.Release(std::unique_ptr<StrongEnemy>(static_cast<StrongEnemy*>(enemy.release())));
    }
}
