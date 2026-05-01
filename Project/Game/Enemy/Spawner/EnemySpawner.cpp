#include "EnemySpawner.h"
#include "Enemy/EnemyManager.h"
#include "MathFunction.h"
#include <cassert>
#include <fstream>
#include <imgui.h>

void EnemySpawner::Init(const std::string& jsonData) {
    ParseJsonData(jsonData);
    SettingGroupSpawnPos();

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
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

    // 最大フェーズ数を取得
    maxFazeNum_ = static_cast<int32_t>(spawnGroups_.size());

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
        if (spawnData.contains("parent_boss_name")) {
            spawn.parentBossName = spawnData["parent_boss_name"];
        }

        spawnPoints_.push_back(spawn);
    }

    // ボス名 → スポーン位置のルックアップテーブルを構築
    bossSpawnPositions_.clear();
    for (const auto& sp : spawnPoints_) {
        if (sp.enemyType == "BossEnemy" || sp.enemyType == "StrongEnemy") {
            bossSpawnPositions_[sp.name] = sp.position;
        }
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

        // 次グループの敵を1フレーム1体ずつ事前生成
        PreGenerateNextGroupEnemy();

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
                if (pEnemyManager_) {
                    // 事前生成済みがあればそれをアクティブ化
                    bool used = pEnemyManager_->ActivateSingleWaitingEnemy(spawn->groupId);
                    if (!used) {
                        // ボスの場合は spawn->name を parentBossName として渡す
                        // ザコの場合は spawn->parentBossName を渡す
                        std::string nameForManager = "";
                        if (spawn->enemyType == "BossEnemy" || spawn->enemyType == "StrongEnemy") {
                            // ボス自身の名前を登録用として渡す
                            nameForManager = spawn->name;
                        } else {
                            // ザコはJSONで決まった親ボス名を渡す
                            nameForManager = spawn->parentBossName;
                        }

                        // JSONのparent_boss_nameからローカルオフセットを計算して渡す
                        Vector3 localOffset = {};
                        if (!spawn->parentBossName.empty()) {
                            auto it = bossSpawnPositions_.find(spawn->parentBossName);
                            if (it != bossSpawnPositions_.end()) {
                                localOffset   = spawn->position - it->second;
                                localOffset.y = 0.0f;
                            }
                        }

                        pEnemyManager_->SpawnEnemy(spawn->enemyType, spawn->position, spawn->groupId,
                            localOffset, nameForManager);
                    }
                }

                spawn->hasSpawned = true;
                group.spawnedCount++;
                group.aliveCount++;
            }
        }
    }
}

///=========================================================
/// パラメータBind
///==========================================================
void EnemySpawner::RegisterParams() {
    for (int32_t i = 0; i < spawnGroups_.size() - 1; ++i) {
        globalParameter_->Regist(groupName_, "nextFazeEnemyNum" + std::to_string(i), &spawnGroups_[i].nextFazeEnemyNum);
    }
}

///=========================================================
/// パラメータ調整
///==========================================================
void EnemySpawner::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        for (int32_t i = 0; i < spawnGroups_.size() - 1; ++i) {
            ImGui::InputInt(("nextFazeEnemyNum" + std::to_string(i)).c_str(), &spawnGroups_[i].nextFazeEnemyNum);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif
}

bool EnemySpawner::IsGroupCompleted(int groupId) const {
    if (groupId >= 0 && groupId < spawnGroups_.size()) {
        const SpawnGroup& group = spawnGroups_[groupId];
        return (group.aliveCount <= group.nextFazeEnemyNum && group.spawnedCount == group.objectCount);
    }
    return false;
}

void EnemySpawner::ActivateNextGroup() {
    currentGroupIndex_++;
    if (currentGroupIndex_ >= static_cast<int>(spawnGroups_.size())) {
        if (shouldLoop_) {
            RestartLoop();
        } else {
            isSystemActive_     = false;
            allGroupsCompleted_ = true;
        }
        return;
    }
}

void EnemySpawner::RestartLoop() {
    for (auto& group : spawnGroups_) {
        group.isActive       = false;
        group.isCompleted    = false;
        group.spawnedCount   = 0;
        group.aliveCount     = 0;
        group.groupStartTime = 0.0f;
    }
    for (auto& spawn : spawnPoints_) {
        spawn.hasSpawned   = false;
        spawn.preGenerated = false;
    }
    currentGroupIndex_  = 0;
    currentTime_        = 0.0f;
    isSystemActive_     = true;
    allGroupsCompleted_ = false;

    // 事前生成された待機中の敵を破棄
    if (pEnemyManager_) {
        pEnemyManager_->ClearAllWaitingEnemies();
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

void EnemySpawner::PreGenerateNextGroupEnemy() {
    int32_t nextGroupId = currentGroupIndex_ + 1;
    if (nextGroupId >= static_cast<int32_t>(spawnGroups_.size())) {
        return; // 次グループなし
    }

    auto it = groupSpawnPoints_.find(nextGroupId);
    if (it == groupSpawnPoints_.end()) {
        return;
    }

    // preGeneratedフラグが立っていないSpawnPointを1つ選んで事前生成
    for (auto* spawn : it->second) {
        if (!spawn->preGenerated) {
            if (pEnemyManager_) {
                //  ボスの場合は spawn->name を、ザコの場合は spawn->parentBossName を渡す
                std::string nameForManager = "";
                if (spawn->enemyType == "BossEnemy" || spawn->enemyType == "StrongEnemy") {
                    nameForManager = spawn->name;
                } else {
                    nameForManager = spawn->parentBossName;
                }

                // JSONのparent_boss_nameからローカルオフセットを計算して渡す
                Vector3 localOffset = {};
                if (!spawn->parentBossName.empty()) {
                    auto bit = bossSpawnPositions_.find(spawn->parentBossName);
                    if (bit != bossSpawnPositions_.end()) {
                        localOffset   = spawn->position - bit->second;
                        localOffset.y = 0.0f;
                    }
                }

                pEnemyManager_->PreGenerateEnemy(spawn->enemyType, spawn->position, nextGroupId,
                    localOffset, nameForManager);
            }
            spawn->preGenerated = true;
            return;
        }
    }
}

void EnemySpawner::SetEnemyManager(EnemyManager* enemyManager) {
    pEnemyManager_ = enemyManager;
}
