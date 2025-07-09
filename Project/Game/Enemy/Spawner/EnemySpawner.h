#pragma once

#include "Enemy/BaseEnemy.h"
#include <array>
#include <cstdint>
#include <json.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class EnemyManager;
class EnemySpawner {
private:
    // スポーンポイント構造体
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

    // グループ情報構造体
    struct SpawnGroup {
        int32_t id;
        float spawnTime;
        int32_t objectCount;
        bool isActive        = false;
        bool isCompleted     = false;
        int32_t spawnedCount = 0;
        int32_t aliveCount   = 0;
        float groupStartTime = 0.0f;
    };

public:
    EnemySpawner()  = default;
    ~EnemySpawner() = default;

    // JSONデータから初期化
    void Init(const std::string& jsonData);

    // 更新処理
    void Update(float deltaTime);

    // 敵が倒されたときの通知
    void OnEnemyDestroyed(int groupId);

   
    ///=======================================================================================
    /// Editor method
    ///=======================================================================================
    void ParseJsonData(const std::string& jsonData);
    void SettingGroupSpawnPos();
    void UpdateCurrentGroup();
    void SpawnEnemiesInGroup(SpawnGroup& group);
    bool IsGroupCompleted(int groupId) const;
    void ActivateNextGroup();

private:
    using json = nlohmann::json;
    json jsonData_;

private:
    // 敵の種類リスト
    std::array<std::string,2> enemyTypes_ = {"NormalEnemy", "StrongEnemy"};
    std::vector<SpawnPoint> spawnPoints_;
    std::vector<SpawnGroup> spawnGroups_;
    std::unordered_map<int, std::vector<SpawnPoint*>> groupSpawnPoints_;

    float currentTime_;
    int currentGroupIndex_;
    bool isSystemActive_;
    bool allGroupsCompleted_;

private:
    EnemyManager* pEnemyManager_ = nullptr;

    const std::string directoryPath_ = "Resources/EnemyParamater/"; // path
 
    public:
    ///=======================================================================================
    /// getter method
    ///=======================================================================================
    bool IsActive() const { return isSystemActive_; }
    int GetCurrentGroupIndex() const { return currentGroupIndex_; }
    int GetTotalGroups() const { return static_cast<int>(spawnGroups_.size()); }
    bool GetAllGroupsCompleted() const { return allGroupsCompleted_; }

    ///=======================================================================================
    /// setter method
    ///=======================================================================================
    void SetEnemyManager(EnemyManager* enemyManager);
};