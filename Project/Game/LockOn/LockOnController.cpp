#include "LockOnController.h"

// target
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/EnemyManager.h"

void LockOnController::Init() {
    lockOn_ = std::make_unique<LockOn>();
    lockOn_->Init();
}
void LockOnController::Update(Player* pPlayer, const KetaEngine::ViewProjection& viewProjection) {

    // より安全で読みやすいバージョン
    std::vector<LockOn::LockOnVariant> targets;
    const auto& enemies = pEnemyManager_->GetEnemies();

    for (const auto& enemy : enemies) {
        if (enemy) {
            targets.emplace_back(enemy.get());
        }
    }

    lockOn_->Update(targets, pPlayer, viewProjection);
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