#pragma once
#include "LockOn/LockOn.h"
#include <memory>

class ViewProjection;
class Player;
class EnemyManager;

/// <summary>
/// ロックオン制御クラス
/// </summary>
class LockOnController {
public:
    LockOnController()  = default;
    ~LockOnController() = default;

    // 初期化
    void Init();

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="pPlayer">プレイヤー</param>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Update(Player* pPlayer, const ViewProjection& viewProjection);

private:
    std::unique_ptr<LockOn> lockOn_ = nullptr;
    EnemyManager* pEnemyManager_    = nullptr;

public:
    // getter
    LockOn* GetLockOn();

    // setter
    void SetEnemyManager(EnemyManager* enemyManager);
};