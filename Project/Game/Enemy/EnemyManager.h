#pragma once

// 3D
#include "3D/ViewProjection.h"
// DamageReaction
#include "DamageReaction/EnemyDamageReactionController.h"
// BaseEnemy
#include "Types/BaseEnemy.h"
#include "Types/NormalEnemy.h"
// HPBar
#include "HPBar/EnemyHPBarColorConfig.h"

/// std
#include <json.hpp>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

enum class Type;
struct BaseEnemy::Parameter;
class Player;
class Combo;
class GameCamera;
class EnemySpawner;
class KillCounter;
class DeathTimer;

class EnemyManager {
public:
    ///========================================================
    /// public method
    ///========================================================

    EnemyManager()  = default;
    ~EnemyManager() = default;

    // 初期化、更新
    void Init();
    void Update();

    // 敵全滅チェック
    void CheckIsEnemiesCleared();

    // 敵の生成
    // JSONで親ボスが決まっている場合に渡す
    void SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
                    const Vector3& localOffset = {}, const std::string& parentBossName = "");
    void HpBarUpdate(const KetaEngine::ViewProjection& viewProjection);

    // 事前生成（描画/更新/当たり判定off、次ウェーブ用）
    //JSONで親ボスが決まっている場合に渡す
    void PreGenerateEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
                          const Vector3& localOffset = {}, const std::string& parentBossName = "");

    // 待機中の敵を1体アクティブ化
    bool ActivateSingleWaitingEnemy(int32_t groupID);

    // 全待機敵を破棄
    void ClearAllWaitingEnemies();

    // Param Edit
    void RegisterParams();
    void AdjustParam();
    void DamageReactionCreate();
    void DrawEnemyParamUI(BaseEnemy::Type type);

    // debugSpawn
    void DebugEnemySpawn();

    // エディター用にアニメーションリストを更新
    void UpdateAvailableAnimationsForEditor(BaseEnemy* enemy);

private:
    // ボス・取り巻きリンク設定
    // JSONで親ボスが決まっている場合に渡す
    void LinkBossAndMinions(int32_t groupID, NormalEnemy* newMinion = nullptr, const std::string& parentBossName = "");

    // ボスとプレイヤーの距離チェック → 煽り状態更新
    void UpdateTauntState();

    // ボスが死んだときの処理
    void OnBossKilled(BaseEnemy* dyingBoss);

private:
    // json
    using json = nlohmann::json;

private:
    // globalParameter
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "Enemies";

    // パラメータ
    std::array<BaseEnemy::Parameter, 2> parameters_;
    NormalEnemy::NormalParameter normalEnemyParam_;

    // ohter class
    Player* pPlayer_;
    GameCamera* pGameCamera_;
    Combo* pCombo_           = nullptr;
    KillCounter* pKillCounter_ = nullptr;
    EnemySpawner* pEnemySpawner_;
    DeathTimer* pDeathTimer_ = nullptr;
    const KetaEngine::ViewProjection* pViewProjection_ = nullptr;

    // damageReaction
    std::unique_ptr<EnemyDamageReactionController> damageReactionController_;

private:
    ///========================================================
    /// Private variants
    ///========================================================

    // 敵リスト
    std::vector<std::unique_ptr<BaseEnemy>> enemies_;
    std::vector<std::string> enemyTypes_ = {"NormalEnemy", "StrongEnemy"};

    // 事前生成待機リスト（グループID → 待機中の敵）
    std::unordered_map<int32_t, std::vector<std::unique_ptr<BaseEnemy>>> waitingEnemies_;

    // ボス別取り巻き管理
    std::unordered_map<BaseEnemy*, std::vector<NormalEnemy*>> minionsByBoss_;

    // ボス名(JSON上の名前) → ランタイムのボスポインタ のルックアップテーブル
    std::unordered_map<std::string, BaseEnemy*> bossByName_;

    // ボスがプレイヤーのこの距離以内に入ったら煽り発動
    float bossPlayerTriggerDistance_ = 15.0f;

    // 一時的な敵生成用データ
    std::string selectedEnemyType_;
    Vector3 spawnPosition_;

    // 敵がすべていなくなったことを示すフラグ
    bool areAllEnemiesCleared_;

    // HPバー色設定
    EnemyHPBarColorConfig hpBarColorConfig_;

    // プレイヤーからのHPバー表示距離
    float hpBarDisplayDistance_ = 20.0f;

    // debugSpawn用
    int32_t selectedEnemyTypeIndex_;

public:
    ///========================================================
    /// getter method
    ///========================================================
    const bool& GetIsAllCleared() const { return areAllEnemiesCleared_; }
    const std::vector<std::unique_ptr<BaseEnemy>>& GetEnemies() const { return enemies_; }
    EnemyDamageReactionController* GetDamageReactionController() const { return damageReactionController_.get(); }

    bool IsAnyEnemyInAnticipation() const;

    ///========================================================
    /// setter method
    ///========================================================
    void SetPlayer(Player* player);
    void SetGameCamera(GameCamera* gameCamera);
    void SetCombo(Combo* combo);
    void SetKillCounter(KillCounter* killCounter);
    void SetEnemySpawner(EnemySpawner* enemySpawner);
    void SetDeathTimer(DeathTimer* deathTimer);
    void SetViewProjection(const KetaEngine::ViewProjection* vp) { pViewProjection_ = vp; }
};
