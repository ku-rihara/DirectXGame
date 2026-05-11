#include "LockOnController.h"

// target
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/StrongEnemy.h"
#include "Enemy/EnemyManager.h"

void LockOnController::Init() {
    lockOn_ = std::make_unique<LockOn>();
    lockOn_->Init();
}
void LockOnController::Update(Player* pPlayer, const KetaEngine::ViewProjection& viewProjection) {

    // ロックオン対象の収集
    std::vector<LockOn::LockOnVariant> targets;
    const auto& enemies = pEnemyManager_->GetEnemies();

    // 強敵のみをロックオン対象とする
    for (const auto& enemy : enemies) {
        if (enemy && dynamic_cast<StrongEnemy*>(enemy.get())) {
            targets.emplace_back(enemy.get());
        }
    }

    lockOn_->Update(targets, pPlayer, viewProjection);
}

void LockOnController::AdjustParam() {
    if (lockOn_) {
        lockOn_->AdjustParam();
    }
}

void LockOnController::SetEnemyManager(EnemyManager* enemyManager) {
    pEnemyManager_ = enemyManager;
}

LockOn* LockOnController::GetLockOn() {
    if (!lockOn_) {
        return nullptr;
    }
    return lockOn_.get();
}