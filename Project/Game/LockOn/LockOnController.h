#pragma once

#include "LockOn/LockOn.h"
#include <memory>

class ViewProjection;
class Player;
class EnemyManager;

class LockOnController {
public:
    LockOnController()  = default;
    ~LockOnController() = default;

    void Init();
    void Update(Player* pPlayer, const ViewProjection& viewProjection);
    void Draw();

private:
    std::unique_ptr<LockOn> lockOn_ = nullptr;
    EnemyManager* pEnemyManager_    = nullptr;

public:
    // ------------------------------------------------------------
    // getter method
    // ------------------------------------------------------------

    LockOn* GetLockOn();

    // ------------------------------------------------------------
    // setter method
    // ------------------------------------------------------------
    void SetEnemyManager(EnemyManager* enemyManager);
};