#include "EnemySpawner.h"
#include "Enemy/EnemyManager.h"
#include "MathFunction.h"
#include <cassert>
#include <fstream>

void EnemySpawner::Init(const std::string& jsonData) {
    ParseJsonData(jsonData);
    SettingGroupSpawnPos();
}

void EnemySpawner::ParseJsonData(const std::string& filename) {
    const std::string fullPath = directoryPath_ + filename;

    std::ifstream ifs(fullPath);
    if (!ifs.is_open()) {
        return;
    }

    // jsonData_に読み込み
    ifs >> jsonData_;

    assert(jsonData_.is_object());
    assert(jsonData_.contains("groups"));
    assert(jsonData_.contains("spawn_points"));

    // グループ情報の読み込み
    spawnGroups_.clear();
    for (const auto& groupData : jsonData_["groups"]) {
        SpawnGroup group;
        group.id          = groupData["id"];
        group.spawnTime   = groupData["spawn_time"];
        group.objectCount = groupData["object_count"];
        spawnGroups_.push_back(group);
    }

    // スポーンポイント情報の読み込み
    spawnPoints_.clear();
    for (const auto& spawnData : jsonData_["spawn_points"]) {
        SpawnPoint spawn;
        spawn.name      = spawnData["name"];
        spawn.groupId   = spawnData["group_id"];
        spawn.spawnTime = spawnData["spawnTime"];

        // pos
        spawn.position.x = (float)spawnData["position"][0];
        spawn.position.y = (float)spawnData["position"][2];
        spawn.position.z = (float)spawnData["position"][1];

        // rotate
        spawn.rotation.x = -ToRadian((float)spawnData["rotation"][0]);
        spawn.rotation.y = -ToRadian((float)spawnData["rotation"][2]);
        spawn.rotation.z = -ToRadian((float)spawnData["rotation"][1]);

        // scale
        spawn.scale.x = (float)spawnData["scaling"][0];
        spawn.scale.y = (float)spawnData["scaling"][2];
        spawn.scale.z = (float)spawnData["scaling"][1];

        // etcParams
        spawn.enemyType   = spawnData["enemy_type"];
        spawn.spawnOffset = spawnData["spawn_offset"];
        
        spawnPoints_.push_back(spawn);
    }
}

void EnemySpawner::SettingGroupSpawnPos() {
    groupSpawnPoints_.clear();

    for (auto& spawn : spawnPoints_) {
        groupSpawnPoints_[spawn.groupId].push_back(&spawn);
    }
}

void EnemySpawner::Update(float deltaTime) {

    currentTime_ += deltaTime;

    if (currentGroupIndex_ < spawnGroups_.size()) {
        UpdateCurrentGroup();
    }
}

void EnemySpawner::UpdateCurrentGroup() {
    SpawnGroup& currentGroup = spawnGroups_[currentGroupIndex_];

    // 進行中グループの確認
    if (!currentGroup.isActive) {
        if (currentGroupIndex_ == 0) {
            // 最初のグループは即座にアクティブ
            currentGroup.isActive       = true;
            currentGroup.groupStartTime = currentTime_;
        } else {
            // 前のグループの全滅を確認
            if (spawnGroups_[currentGroupIndex_ - 1].isCompleted) {
                currentGroup.isActive       = true;
                currentGroup.groupStartTime = currentTime_;
            }
        }
    }

    // 進行中グループの敵をスポーン
    if (currentGroup.isActive && !currentGroup.isCompleted) {
        SpawnEnemiesInGroup(currentGroup);

        // グループが全滅したかチェック
        if (IsGroupCompleted(currentGroup.id)) {
            currentGroup.isCompleted = true;
            ActivateNextGroup();
        }
    }
}

void EnemySpawner::SpawnEnemiesInGroup(SpawnGroup& group) {
    float groupElapsedTime = currentTime_ - group.groupStartTime;

    auto& groupSpawns = groupSpawnPoints_[group.id];

    for (auto* spawn : groupSpawns) {
        if (!spawn->hasSpawned) {
            float adjustedSpawnTime = spawn->spawnTime + spawn->spawnOffset;

            if (groupElapsedTime >= adjustedSpawnTime) {
                // 敵をスポーン
                if (pEnemyManager_) {
                    pEnemyManager_->SpawnEnemy(spawn->enemyType, spawn->position, spawn->groupId);
                }

                spawn->hasSpawned = true;
                group.spawnedCount++;
                group.aliveCount++;
            }
        }
    }
}

bool EnemySpawner::IsGroupCompleted(int groupId) const {
    if (groupId >= 0 && groupId < spawnGroups_.size()) {
        const SpawnGroup& group = spawnGroups_[groupId];
        return (group.aliveCount == 0 && group.spawnedCount == group.objectCount);
    }
    return false;
}

void EnemySpawner::ActivateNextGroup() {
    currentGroupIndex_++;
    if (currentGroupIndex_ >= spawnGroups_.size()) {
        // 全てのグループが全滅
        isSystemActive_     = false;
        allGroupsCompleted_ = true;
    }
}

void EnemySpawner::OnEnemyDestroyed(int groupId) {
    if (groupId >= 0 && groupId < spawnGroups_.size()) {
        spawnGroups_[groupId].aliveCount--;
        if (spawnGroups_[groupId].aliveCount < 0) {
            spawnGroups_[groupId].aliveCount = 0;
        }
    }
}

void EnemySpawner::SetEnemyManager(EnemyManager* enemyManager) {
    pEnemyManager_ = enemyManager;
}