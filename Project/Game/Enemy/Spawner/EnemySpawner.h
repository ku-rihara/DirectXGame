#pragma once

#include "Enemy/Types/BaseEnemy.h"
#include <array>
#include <cstdint>
#include <json.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class EnemyManager;

/// <summary>
/// 敵の生成管理クラス
/// </summary>
class EnemySpawner {
private:
    struct SpawnPoint {
        std::string name;
        int32_t groupId;
        float spawnTime;
        Vector3 position;
        Vector3 rotation;
        Vector3 scale;
        std::string enemyType;
        float spawnOffset;
        bool hasSpawned = false;
    };

    struct SpawnGroup {
        int32_t id;
        int32_t objectCount;
        int32_t spawnedCount     = 0;
        int32_t aliveCount       = 0;
        int32_t nextFazeEnemyNum = 0;
        bool isActive            = false;
        bool isCompleted         = false;
        float spawnTime;
        float groupStartTime = 0.0f;
    };

public:
    EnemySpawner()  = default;
    ~EnemySpawner() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="jsonData">スポーン設定のJSONデータ</param>
    void Init(const std::string& jsonData);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(float deltaTime);

    /// <summary>
    /// 敵が倒されたときの通知
    /// </summary>
    /// <param name="groupId">グループID</param>
    void OnEnemyDestroyed(int groupId);

    ///=======================================================================================
    /// Editor method
    ///=======================================================================================

    /// <summary>
    /// JSONデータの解析
    /// </summary>
    /// <param name="jsonData">JSONデータ</param>
    void ParseJsonData(const std::string& jsonData);

    /// <summary>
    /// グループ内の敵を生成
    /// </summary>
    /// <param name="group">スポーングループ</param>
    void SpawnEnemiesInGroup(SpawnGroup& group);

    /// <summary>
    /// グループが完了したかチェック
    /// </summary>
    /// <param name="groupId">グループID</param>
    /// <returns>完了していればtrue</returns>
    bool IsGroupCompleted(int groupId) const;

    void ActivateNextGroup(); //< 次のグループをアクティブ化
    void SettingGroupSpawnPos(); //< グループスポーン位置設定
    void UpdateCurrentGroup(); //< 現在のグループ更新

    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

private:
    using json = nlohmann::json;
    json jsonData_;

    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "EnemySpawner";

    EnemyManager* pEnemyManager_     = nullptr;
    const std::string directoryPath_ = "Resources/EnemyParameter/";

private:
    std::array<std::string, 2> enemyTypes_ = {"NormalEnemy", "StrongEnemy"};
    std::vector<SpawnPoint> spawnPoints_;
    std::vector<SpawnGroup> spawnGroups_;
    std::unordered_map<int, std::vector<SpawnPoint*>> groupSpawnPoints_;

    int32_t maxFazeNum_;

    float currentTime_;
    int currentGroupIndex_;
    bool isSystemActive_;
    bool allGroupsCompleted_;

public:
    ///=======================================================================================
    /// getter method
    ///=======================================================================================
    const bool& IsActive() const { return isSystemActive_; }
    int GetCurrentGroupIndex() const { return currentGroupIndex_; }
    int GetTotalGroups() const { return static_cast<int>(spawnGroups_.size()); }
    const bool& GetAllGroupsCompleted() const { return allGroupsCompleted_; }

    ///=======================================================================================
    /// setter method
    ///=======================================================================================
    void SetEnemyManager(EnemyManager* enemyManager);
};