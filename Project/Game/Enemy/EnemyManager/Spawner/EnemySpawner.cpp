#include "EnemySpawner.h"
#include "Enemy/EnemyManager/EnemyManager.h"
#include "Enemy/EnemyManager/Parameter/EnemyParameter.h"
#include "Enemy/Types/NormalEnemy.h"
#include "Enemy/Types/StrongEnemy.h"
#include "MathFunction.h"
#include <cassert>
#include <format>
#include <fstream>
#include <imgui.h>

///========================================================================================
///  初期化
///========================================================================================
void EnemySpawner::Init(const std::string& jsonFile, EnemyManager* manager) {
    pEnemyManager_ = manager;

    ParseJsonData(jsonFile);
    SettingGroupSpawnPos();

    for (auto& group : spawnGroups_) {
        group.strongEnemyCount = 0;
        auto it = groupSpawnPoints_.find(group.id);
        if (it != groupSpawnPoints_.end()) {
            for (auto* sp : it->second) {
                if (sp->enemyType == "StrongEnemy" || sp->enemyType == "BossEnemy") {
                    group.strongEnemyCount++;
                }
            }
        }
    }

    shouldLoop_ = true;

    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

///========================================================================================
///  敵の生成
///========================================================================================
void EnemySpawner::SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
    const Vector3& localOffset, const std::string& parentBossName) {

    BaseEnemy::Type type;
    if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]) {
        type = BaseEnemy::Type::NORMAL;
    } else if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)] || enemyType == "BossEnemy") {
        type = BaseEnemy::Type::STRONG;
    } else {
        return;
    }

    auto enemy = pEnemyManager_->AcquireFromPool(type);
    if (!enemy) {
        return;
    }

    auto* param = pEnemyManager_->GetParam();
    enemy->GetBaseInfo()->SetParameter(type, param->GetBaseParam(type));
    if (type == BaseEnemy::Type::STRONG) {
        static_cast<StrongEnemy*>(enemy.get())->SetStrongParameter(param->GetStrongParam());
    }

    enemy->RefreshCollision();
    enemy->GetBaseInfo()->SetPlayer(pEnemyManager_->GetPlayer());
    enemy->GetBaseInfo()->SetGameCamera(pEnemyManager_->GetGameCamera());
    enemy->GetBaseInfo()->SetManager(pEnemyManager_);
    enemy->GetBaseInfo()->SetCombo(pEnemyManager_->GetCombo());
    enemy->GetBaseInfo()->SetKillCounter(pEnemyManager_->GetKillCounter());
    enemy->SetGroupId(groupID);
    enemy->Init(position);
    enemy->SetHPBarColorConfig(pEnemyManager_->GetHPBarColorConfig());
    pEnemyManager_->UpdateAvailableAnimationsForEditor(enemy.get());

    if (type == BaseEnemy::Type::NORMAL) {
        NormalEnemy* ne = static_cast<NormalEnemy*>(enemy.get());
        ne->SetSpawnOffset(localOffset);
        ne->SetNormalParameter(param->GetNormalParam());
        ne->SetOnDamageTakenCallback([mgr = pEnemyManager_, ne]() {
            mgr->OnNormalEnemyDamaged(ne);
        });
    }

    pEnemyManager_->RegisterEnemy(std::move(enemy), groupID, parentBossName);
}

///========================================================================================
///  事前生成
///========================================================================================
void EnemySpawner::PreGenerateEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
    const Vector3& localOffset, const std::string& parentBossName) {

    BaseEnemy::Type type;
    if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::NORMAL)]) {
        type = BaseEnemy::Type::NORMAL;
    } else if (enemyType == enemyTypes_[static_cast<size_t>(BaseEnemy::Type::STRONG)] || enemyType == "BossEnemy") {
        type = BaseEnemy::Type::STRONG;
    } else {
        return;
    }

    auto enemy = pEnemyManager_->AcquireFromPool(type);
    if (!enemy) {
        return;
    }

    auto* param = pEnemyManager_->GetParam();
    enemy->GetBaseInfo()->SetParameter(type, param->GetBaseParam(type));
    if (type == BaseEnemy::Type::STRONG) {
        static_cast<StrongEnemy*>(enemy.get())->SetStrongParameter(param->GetStrongParam());
    }

    enemy->RefreshCollision();
    enemy->GetBaseInfo()->SetPlayer(pEnemyManager_->GetPlayer());
    enemy->GetBaseInfo()->SetGameCamera(pEnemyManager_->GetGameCamera());
    enemy->GetBaseInfo()->SetManager(pEnemyManager_);
    enemy->GetBaseInfo()->SetCombo(pEnemyManager_->GetCombo());
    enemy->GetBaseInfo()->SetKillCounter(pEnemyManager_->GetKillCounter());
    enemy->SetGroupId(groupID);
    enemy->Init(position);
    enemy->SetHPBarColorConfig(pEnemyManager_->GetHPBarColorConfig());
    pEnemyManager_->UpdateAvailableAnimationsForEditor(enemy.get());

    if (type == BaseEnemy::Type::NORMAL) {
        NormalEnemy* ne = static_cast<NormalEnemy*>(enemy.get());
        ne->SetSpawnOffset(localOffset);
        ne->SetNormalParameter(param->GetNormalParam());
        ne->SetOnDamageTakenCallback([mgr = pEnemyManager_, ne]() {
            mgr->OnNormalEnemyDamaged(ne);
        });
        ne->SetParentBossName(parentBossName);
    }

    enemy->SetScale(Vector3::ZeroVector());
    enemy->GetAnimator()->SetAnimationActive(false);
    enemy->SetIsAdaptCollision(false);

    std::string bossRegName = "";
    if (type == BaseEnemy::Type::STRONG) {
        bossRegName = parentBossName;
    }
    pEnemyManager_->RegisterWaitingEnemy(std::move(enemy), groupID, bossRegName);
}

///========================================================================================
///  JSONデータ解析
///========================================================================================
void EnemySpawner::ParseJsonData(const std::string& filename) {
    const std::string fullPath = directoryPath_ + filename;

    std::ifstream ifs(fullPath);
    if (!ifs.is_open()) {
        return;
    }

    ifs >> jsonData_;

    assert(jsonData_.is_object());
    assert(jsonData_.contains("groups"));
    assert(jsonData_.contains("spawn_points"));

    spawnGroups_.clear();
    for (const auto& groupData : jsonData_["groups"]) {
        SpawnGroup group;
        group.id          = groupData["id"];
        group.spawnTime   = groupData["spawn_time"];
        group.objectCount = groupData["object_count"];
        spawnGroups_.push_back(group);
    }

    maxFazeNum_ = static_cast<int32_t>(spawnGroups_.size());

    spawnPoints_.clear();
    for (const auto& spawnData : jsonData_["spawn_points"]) {
        SpawnPoint spawn;
        spawn.name      = spawnData["name"];
        spawn.groupId   = spawnData["group_id"];
        spawn.spawnTime = spawnData["spawnTime"];

        spawn.position.x = (float)spawnData["position"][0];
        spawn.position.y = (float)spawnData["position"][2];
        spawn.position.z = (float)spawnData["position"][1];

        spawn.rotation.x = -ToRadian((float)spawnData["rotation"][0]);
        spawn.rotation.y = -ToRadian((float)spawnData["rotation"][2]);
        spawn.rotation.z = -ToRadian((float)spawnData["rotation"][1]);

        spawn.scale.x = (float)spawnData["scaling"][0];
        spawn.scale.y = (float)spawnData["scaling"][2];
        spawn.scale.z = (float)spawnData["scaling"][1];

        spawn.enemyType   = spawnData["enemy_type"];
        spawn.spawnOffset = spawnData["spawn_offset"];
        if (spawnData.contains("parent_boss_name")) {
            spawn.parentBossName = spawnData["parent_boss_name"];
        }

        spawnPoints_.push_back(spawn);
    }

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

///========================================================================================
///  更新
///========================================================================================
void EnemySpawner::Update(float deltaTime) {
    currentTime_ += deltaTime;
    ++preGenFrameCount_;

    if (currentGroupIndex_ < static_cast<int>(spawnGroups_.size())) {
        UpdateCurrentGroup();
    }
}

void EnemySpawner::UpdateCurrentGroup() {
    SpawnGroup& currentGroup = spawnGroups_[currentGroupIndex_];

    if (!currentGroup.isActive) {
        if (currentGroupIndex_ == 0) {
            if (preGenFrameCount_ % kPreGenFrameInterval == 0) {
                PreGenerateCurrentGroupEnemy();
            }
            if (IsGroupFullyPreGenerated(0)) {
                currentGroup.isActive       = true;
                currentGroup.groupStartTime = currentTime_;
            }
        } else {
            if (spawnGroups_[currentGroupIndex_ - 1].isCompleted) {
                currentGroup.isActive       = true;
                currentGroup.groupStartTime = currentTime_;
            }
        }
    }

    if (currentGroup.isActive && !currentGroup.isCompleted) {
        SpawnEnemiesInGroup(currentGroup);

        if (preGenFrameCount_ % kPreGenFrameInterval == 0) {
            PreGenerateCurrentGroupEnemy();
            PreGenerateNextGroupEnemy();
        }

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
                bool used = pEnemyManager_->ActivateSingleWaitingEnemy(spawn->groupId);
                if (!used) {
                    std::string nameForManager = "";
                    if (spawn->enemyType == "BossEnemy" || spawn->enemyType == "StrongEnemy") {
                        nameForManager = spawn->name;
                    } else {
                        nameForManager = spawn->parentBossName;
                    }

                    Vector3 localOffset = {};
                    if (!spawn->parentBossName.empty()) {
                        auto it = bossSpawnPositions_.find(spawn->parentBossName);
                        if (it != bossSpawnPositions_.end()) {
                            localOffset   = spawn->position - it->second;
                            localOffset.y = 0.0f;
                        }
                    }

                    SpawnEnemy(spawn->enemyType, spawn->position, spawn->groupId, localOffset, nameForManager);
                }

                spawn->hasSpawned = true;
                group.spawnedCount++;
                group.aliveCount++;
            }
        }
    }
}

///========================================================================================
///  パラメータBind / 調整
///========================================================================================
void EnemySpawner::RegisterParams() {
    for (int32_t i = 0; i < static_cast<int32_t>(spawnGroups_.size()) - 1; ++i) {
        globalParameter_->Regist(groupName_, std::format("nextFazeEnemyNum{}", i), &spawnGroups_[i].nextFazeEnemyNum);
    }
}

void EnemySpawner::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        for (int32_t i = 0; i < static_cast<int32_t>(spawnGroups_.size()) - 1; ++i) {
            ImGui::InputInt(std::format("nextFazeEnemyNum{}", i).c_str(), &spawnGroups_[i].nextFazeEnemyNum);
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif
}

///========================================================================================
///  グループ完了チェック / 次グループ起動
///========================================================================================
bool EnemySpawner::IsGroupFullyPreGenerated(int32_t groupId) const {
    auto it = groupSpawnPoints_.find(groupId);
    if (it == groupSpawnPoints_.end()) {
        return true;
    }
    for (const auto* spawn : it->second) {
        if (!spawn->preGenerated) {
            return false;
        }
    }
    return true;
}

bool EnemySpawner::IsGroupCompleted(int groupId) const {
    if (groupId >= 0 && groupId < static_cast<int>(spawnGroups_.size())) {
        const SpawnGroup& group = spawnGroups_[groupId];
        return (group.strongEnemyCount <= 0 && group.spawnedCount == group.objectCount);
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
    }
}

void EnemySpawner::RestartLoop() {
    ParseJsonData(jsonData_.dump());

    for (auto& group : spawnGroups_) {
        group.isActive       = false;
        group.isCompleted    = false;
        group.spawnedCount   = 0;
        group.aliveCount     = 0;
        group.groupStartTime = 0.0f;

        group.strongEnemyCount = 0;
        auto it = groupSpawnPoints_.find(group.id);
        if (it != groupSpawnPoints_.end()) {
            for (auto* sp : it->second) {
                if (sp->enemyType == "StrongEnemy" || sp->enemyType == "BossEnemy") {
                    group.strongEnemyCount++;
                }
            }
        }
    }
    for (auto& spawn : spawnPoints_) {
        spawn.hasSpawned   = false;
        spawn.preGenerated = false;
    }
    currentGroupIndex_  = 0;
    currentTime_        = 0.0f;
    preGenFrameCount_   = 0;
    isSystemActive_     = true;
    allGroupsCompleted_ = false;

    if (pEnemyManager_) {
        pEnemyManager_->ClearAllWaitingEnemies();
    }
}

void EnemySpawner::OnEnemyDestroyed(int groupId) {
    if (groupId >= 0 && groupId < static_cast<int>(spawnGroups_.size())) {
        spawnGroups_[groupId].aliveCount = (std::max)(0, spawnGroups_[groupId].aliveCount - 1);
    }
}

void EnemySpawner::OnStrongEnemyDestroyed(int groupId) {
    if (groupId >= 0 && groupId < static_cast<int>(spawnGroups_.size())) {
        spawnGroups_[groupId].strongEnemyCount = (std::max)(0, spawnGroups_[groupId].strongEnemyCount - 1);
    }
}

///========================================================================================
///  事前生成ヘルパー
///========================================================================================
void EnemySpawner::PreGenerateNextGroupEnemy() {
    int32_t nextGroupId = currentGroupIndex_ + 1;
    if (nextGroupId >= static_cast<int32_t>(spawnGroups_.size())) {
        return;
    }

    auto it = groupSpawnPoints_.find(nextGroupId);
    if (it == groupSpawnPoints_.end()) {
        return;
    }

    for (auto* spawn : it->second) {
        if (!spawn->preGenerated) {
            std::string nameForManager = "";
            if (spawn->enemyType == "BossEnemy" || spawn->enemyType == "StrongEnemy") {
                nameForManager = spawn->name;
            } else {
                nameForManager = spawn->parentBossName;
            }

            Vector3 localOffset = {};
            if (!spawn->parentBossName.empty()) {
                auto bit = bossSpawnPositions_.find(spawn->parentBossName);
                if (bit != bossSpawnPositions_.end()) {
                    localOffset   = spawn->position - bit->second;
                    localOffset.y = 0.0f;
                }
            }

            PreGenerateEnemy(spawn->enemyType, spawn->position, nextGroupId, localOffset, nameForManager);
            spawn->preGenerated = true;
            return;
        }
    }
}

void EnemySpawner::PreGenerateCurrentGroupEnemy() {
    int32_t groupId = currentGroupIndex_;

    auto it = groupSpawnPoints_.find(groupId);
    if (it == groupSpawnPoints_.end()) {
        return;
    }

    for (auto* spawn : it->second) {
        if (!spawn->preGenerated && !spawn->hasSpawned) {
            std::string nameForManager = "";
            if (spawn->enemyType == "BossEnemy" || spawn->enemyType == "StrongEnemy") {
                nameForManager = spawn->name;
            } else {
                nameForManager = spawn->parentBossName;
            }

            Vector3 localOffset = {};
            if (!spawn->parentBossName.empty()) {
                auto bit = bossSpawnPositions_.find(spawn->parentBossName);
                if (bit != bossSpawnPositions_.end()) {
                    localOffset   = spawn->position - bit->second;
                    localOffset.y = 0.0f;
                }
            }

            PreGenerateEnemy(spawn->enemyType, spawn->position, groupId, localOffset, nameForManager);
            spawn->preGenerated = true;
            return;
        }
    }
}
