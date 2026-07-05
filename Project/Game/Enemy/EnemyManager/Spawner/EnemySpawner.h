#pragma once

#include "Enemy/Types/BaseEnemy.h"
#include <array>
#include <cstdint>
#include <json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class EnemyManager;

/// <summary>
/// 敵の生成管理クラス
/// </summary>
class EnemySpawner {
private:
    // スポーンポイントの情報
    struct SpawnPoint {
        std::string name;
        int32_t groupId;
        float spawnTime;
        Vector3 position;
        Vector3 rotation;
        Vector3 scale;
        std::string enemyType;
        float spawnOffset;
        std::string parentBossName;
        bool hasSpawned = false;
    };

    // スポーンする敵のグループ情報
    struct SpawnGroup {
        int32_t id;
        int32_t objectCount;
        int32_t spawnedCount     = 0;
        int32_t aliveCount       = 0;
        int32_t LeaderEnemyCount = 0;
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
    /// JSONファイルからスポーンポイントとグループ情報を読み込み、初期化する
    /// </summary>
    /// <param name="jsonFile">JSONファイルのパス</param>
    /// <param name="manager">EnemyManagerのインスタンス</param>
    void Init(const std::string& jsonFile, EnemyManager* manager);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">前回更新からの経過時間</param>
    void Update(float deltaTime);

    /// <summary>
    /// 敵の生成
    /// </summary>
    /// <param name="enemyType">敵の種類</param>
    /// <param name="position">スポーン位置</param>
    /// <param name="groupID">グループID</param>
    /// <param name="localOffset">ローカルオフセット</param>
    /// <param name="parentBossName">親ボス名</param>
    void SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
        const Vector3& localOffset = {}, const std::string& parentBossName = "");

    /// <summary>
    /// 敵が倒されたときの通知
    /// </summary>
    /// <param name="groupId">グループID</param>
    void OnEnemyDestroyed(int groupId);
    void OnLeaderEnemyDestroyed(int groupId);

    // パラメータ編集
    void AdjustParam();

private:
    /// <summary>
    /// JSONデータを解析し、スポーンポイントとグループ情報を初期化する
    /// </summary>
    /// <param name="jsonData">JSONデータ</param>
    void ParseJsonData(const std::string& jsonData);

    /// <summary>
    /// 指定されたグループのスポーンポイントに基づいて敵を生成する
    /// </summary>
    /// <param name="group">スポーングループ</param>
    void SpawnEnemiesInGroup(SpawnGroup& group);

    /// <summary>
    /// 指定されたグループが全ての敵をスポーンし、かつ全ての敵が倒されたかをチェックする
    /// </summary>
    /// <param name="groupId">グループID</param>
    /// <returns></returns>
    bool IsGroupCompleted(int groupId) const;

    // 次のグループをアクティブ化
    void ActivateNextGroup();
    // グループのスポーン位置を設定
    void SettingGroupSpawnPos();
    // 現在のグループのスポーン位置を設定
    void UpdateCurrentGroup();

    // パラメータ登録とループ再開
    void RegisterParams();
    void RestartLoop();

private:
    // JSONデータ
    using json = nlohmann::json;
    json jsonData_;

    // GlobalParameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "EnemySpawner";

    // 敵マネージャーと敵配置データのディレクトリパス
    EnemyManager* pEnemyManager_     = nullptr;
    const std::string directoryPath_ = "Resources/EnemyParameter/";

    // 敵タイプのリスト
    std::array<std::string, 2> enemyTypes_ = {"EntourageEnemy", "LeaderEnemy"};
    // スポーンポイントとグループ情報
    std::vector<SpawnPoint> spawnPoints_;
    std::vector<SpawnGroup> spawnGroups_;
    // グループIDとスポーンポイントの紐付け
    std::unordered_map<int, std::vector<SpawnPoint*>> groupSpawnPoints_;
    // ボスの名前とスポーン位置の紐付け
    std::unordered_map<std::string, Vector3> bossSpawnPositions_;

    // 最大フェーズ数
    int32_t maxFazeNum_;

    // 現在のスポーン状況
    float currentTime_       = 0.0f;
    int currentGroupIndex_   = 0;
    bool isSystemActive_     = false;
    bool allGroupsCompleted_ = false;
    bool shouldLoop_         = false;

public:
    bool IsActive() const { return isSystemActive_; }
    int GetCurrentGroupIndex() const { return currentGroupIndex_; }
    int GetTotalGroups() const { return static_cast<int>(spawnGroups_.size()); }
    bool GetAllGroupsCompleted() const { return allGroupsCompleted_; }

    void SetShouldLoop(bool loop) { shouldLoop_ = loop; }
};
