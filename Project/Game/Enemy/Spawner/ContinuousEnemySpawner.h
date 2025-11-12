#pragma once

#include "../math/MinMax.h"
#include "Enemy/BaseEnemy.h"
#include <cstdint>
#include <string>
#include <vector>

class EnemyManager;
class Player;
class ViewProjection;

/// <summary>
/// 恒常的な敵出現管理クラス
/// </summary>
class ContinuousEnemySpawner {
private:
    struct SpawnConfig {
        bool isEnabled = true;
        float spawnInterval;
        V2MinMax spawnRandomOffset;
        float minDistanceFromEnemies;
        float fieldMargin;
        int32_t maxRetryCount;
        int32_t maxSpawnCount = -1;
        std::string enemyType = "NormalEnemy";
    };

public:
    ContinuousEnemySpawner()  = default;
    ~ContinuousEnemySpawner() = default;

    // 初期化、更新
    void Init();
    void Update(const float& deltaTime);

    void Start(); //< スポーンシステムの開始
    void Stop(); //< スポーンシステムの停止
    void ResetSpawnCount(); //< スポーンカウントのリセット

    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

private:
    /// <summary>
    /// プレイヤー正面方向ベースのスポーン位置計算
    /// </summary>
    /// <param name="outPosition">スポーン位置出力</param>
    /// <returns>スポーン位置</returns>
    Vector3 CalculateSpawnPosition();

    /// <summary>
    /// スポーン位置が有効かチェック
    /// </summary>
    bool IsValidSpawnPosition(const Vector3& position) const;

    /// <summary>
    /// フィールド内に位置を制限
    /// </summary>
    /// <param name="position">チェック・補正する位置</param>
    /// <returns>フィールド内ならtrue</returns>
    bool ClampToFieldBounds(Vector3& position) const;

  

private:
    GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_      = "ContinuousEnemySpawner";

    EnemyManager* pEnemyManager_    = nullptr;
    Player* pPlayer_                = nullptr;
    const Vector3* pPlayerPosition_ = nullptr;

    // スポーン管理
    SpawnConfig config_;
    float spawnTimer_          = 0.0f;
    int32_t totalSpawnedCount_ = 0; // 累計スポーン数
    bool isActive_             = false;

    // グループID
    static constexpr int32_t kContinuousSpawnGroupId = -1;

public:
    ///=======================================================================================
    /// getter
    ///=======================================================================================
    const bool& IsActive() const { return isActive_; }
    const int32_t& GetTotalSpawnedCount() const { return totalSpawnedCount_; }
    const float& GetNextSpawnTime() const { return config_.spawnInterval - spawnTimer_; }
    SpawnConfig& GetConfig() { return config_; }

    ///=======================================================================================
    /// setter
    ///=======================================================================================
    void SetEnemyManager(EnemyManager* enemyManager) { pEnemyManager_ = enemyManager; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
    void SetPlayerPosition(const Vector3* playerPos) { pPlayerPosition_ = playerPos; }
  
};