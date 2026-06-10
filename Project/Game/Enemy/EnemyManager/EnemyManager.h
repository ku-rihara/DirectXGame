#pragma once

// 3D
#include "3D/ViewProjection.h"
// DamageReaction
#include "DamageReaction/EnemyDamageReactionController.h"
// Parameter
#include "Parameter/EnemyParameter.h"
#include "Enemy/AttackBomb/BossAttackBombParameter.h"
// Pool
#include "Pool/EnemyPool.h"
// Spawner
#include "Spawner/EnemySpawner.h"
// BaseEnemy
#include "Enemy/Types/BaseEnemy.h"
#include "Enemy/Types/EntourageEnemy.h"
#include "Enemy/Types/LeaderEnemy.h"
// HPBar
#include "Enemy/UIs/HPBar/EnemyHPBarColorConfig.h"

/// std
#include <array>
#include <json.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Player;
class Combo;
class GameCamera;
class KillCounter;
class StressGauge;

/// <summary>
/// 敵を管理するクラス
/// </summary>
class EnemyManager {
public:
    EnemyManager()  = default;
    ~EnemyManager();

    ///========================================================
    /// public method
    ///========================================================

    // 初期化、更新
    void Init();
    void Update();
    void UIUpdate(const KetaEngine::ViewProjection& viewProjection);

    // EnemySpawnerを生成・初期化
    void InitSpawner(const std::string& jsonFile);

    // 敵の生成
    void SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
        const Vector3& localOffset = {}, const std::string& parentBossName = "");

    // 敵全滅チェック
    void CheckIsEnemiesCleared();

    // 待機中の敵を1体アクティブ化
    bool ActivateSingleWaitingEnemy(int32_t groupID);

    // 全待機敵を破棄
    void ClearAllWaitingEnemies();

    // Param Edit
    void AdjustParam();
    void DamageReactionCreate();

    // debugSpawn
    void DebugEnemySpawn();

    // エディター用にアニメーションリストを更新
    void UpdateAvailableAnimationsForEditor(BaseEnemy* enemy);

    // EntourageEnemyがダメージを受けたときのコールバック用
    void OnEntourageEnemyDamaged(EntourageEnemy* ne);

    /// <summary>
    /// 敵のオブジェクトプールから1体取得
    /// </summary>
    /// <param name="type">敵の種類</param>
    /// <returns></returns>
    std::unique_ptr<BaseEnemy> AcquireFromPool(BaseEnemy::Type type);

    /// <summary>
    /// 敵を登録
    /// </summary>
    /// <param name="enemy"> 登録する敵</param>
    /// <param name="groupID"> グループID</param>
    /// <param name="bossName"> ボス名</param>
    void RegisterEnemy(std::unique_ptr<BaseEnemy> enemy, int32_t groupID, const std::string& bossName);

    /// <summary>
    /// 敵を待機リストに登録
    /// </summary>
    /// <param name="enemy"> 登録する敵</param>
    /// <param name="groupID"> グループID</param>
    /// <param name="bossName"> ボス名</param>
    void RegisterWaitingEnemy(std::unique_ptr<BaseEnemy> enemy, int32_t groupID, const std::string& bossName = "");

private:
    /// <summary>
    /// ボスとザコ敵の紐付け
    /// </summary>
    /// <param name="groupID"> グループID</param>
    /// <param name="newMinion"> 新しいザコ敵</param>
    /// <param name="parentBossName"> 親ボス名</param>
    void LinkBossAndMinions(int32_t groupID, EntourageEnemy* newMinion = nullptr, const std::string& parentBossName = "");

    // ザコ敵の状態更新
    void UpdateTauntState();

    /// <summary>
    /// ボスが倒されたときの処理
    /// </summary>
    /// <param name="dyingBoss"> 倒されたボス</param>
    void OnBossKilled(BaseEnemy* dyingBoss);

private:
    // json
    using json = nlohmann::json;

    // globalParameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "Enemies";

    // 敵のパラメータ、スポーン管理
    std::unique_ptr<EnemyParameter>          param_;
    std::unique_ptr<BossAttackBombParameter> bombParam_;
    std::unique_ptr<EnemySpawner>            spawner_;

    // 他クラスのポインタ
    Player* pPlayer_                                   = nullptr;
    GameCamera* pGameCamera_                           = nullptr;
    Combo* pCombo_                                     = nullptr;
    KillCounter* pKillCounter_                         = nullptr;
    StressGauge* pStressGauge_                           = nullptr;
    const KetaEngine::ViewProjection* pViewProjection_ = nullptr;

    // ダメージリアクション管理
    std::unique_ptr<EnemyDamageReactionController> damageReactionController_;

    // 敵のリスト、敵タイプのリスト
    std::vector<std::unique_ptr<BaseEnemy>> enemies_;
    // damageReactionController_より後に宣言することで、プール内の敵を先に破棄する
    std::unique_ptr<EnemyPool> pool_;
    std::vector<std::string> enemyTypes_ = {"EntourageEnemy", "LeaderEnemy"};

    // 待機中の敵のリスト
    std::unordered_map<int32_t, std::vector<std::unique_ptr<BaseEnemy>>> waitingEnemies_;
    // ボスとザコ敵の紐付け
    std::unordered_map<BaseEnemy*, std::vector<EntourageEnemy*>> minionsByBoss_;
    // ボスの名前とポインタの紐付け
    std::unordered_map<std::string, BaseEnemy*> bossByName_;
    // ボスとグループIDの紐付け
    std::unordered_map<BaseEnemy*, int32_t> bossGroupIndex_;
    // 次のボスが属するグループID
    int32_t nextBossGroupIndex_ = 0;

    // デバッグ用スポーン位置と敵タイプ
    std::string selectedEnemyType_;
    // デバッグ用スポーン位置
    Vector3 spawnPosition_;
    // デバッグ用敵タイプのインデックス
    int32_t selectedEnemyTypeIndex_ = 0;
    // 敵全滅フラグ
    bool areAllEnemiesCleared_ = false;

    // HPバーの色設定
    EnemyHPBarColorConfig hpBarColorConfig_;

public:
    ///========================================================
    /// Getter
    ///========================================================
    bool GetIsAllCleared() const { return areAllEnemiesCleared_; }
    const std::vector<std::unique_ptr<BaseEnemy>>& GetEnemies() const { return enemies_; }
    EnemyDamageReactionController* GetDamageReactionController() const { return damageReactionController_.get(); }
    float GetHpBarDisplayDistance() const { return param_->GetHpBarDisplayDistance(); }
    float GetUIocclusionRadius() const { return param_->GetUiOcclusionRadius(); }
    EnemySpawner* GetSpawner() const { return spawner_.get(); }
    EnemyParameter*          GetParam()     const { return param_.get(); }
    BossAttackBombParameter* GetBombParam() const { return bombParam_.get(); }

    bool IsAnyEnemyInAnticipation() const;

    // EnemySpawner向けゲッター
    Player* GetPlayer() const { return pPlayer_; }
    GameCamera* GetGameCamera() const { return pGameCamera_; }
    Combo* GetCombo() const { return pCombo_; }
    KillCounter* GetKillCounter() const { return pKillCounter_; }
    StressGauge* GetStressGauge() const { return pStressGauge_; }
    EnemyHPBarColorConfig* GetHPBarColorConfig() { return &hpBarColorConfig_; }

    ///========================================================
    /// Setter
    ///========================================================
    void SetPlayer(Player* player);
    void SetGameCamera(GameCamera* gameCamera);
    void SetCombo(Combo* combo);
    void SetKillCounter(KillCounter* killCounter);
    void SetStressGauge(StressGauge* StressGauge);
    void SetViewProjection(const KetaEngine::ViewProjection* vp) { pViewProjection_ = vp; }
};
