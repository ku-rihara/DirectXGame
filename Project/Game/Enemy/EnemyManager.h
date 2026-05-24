#pragma once

// 3D
#include "3D/ViewProjection.h"
// DamageReaction
#include "DamageReaction/EnemyDamageReactionController.h"
// BaseEnemy
#include "Types/BaseEnemy.h"
#include "Types/NormalEnemy.h"
#include "Types/StrongEnemy.h" 
// HPBar
#include "UIs/HPBar/EnemyHPBarColorConfig.h"

/// std
#include <array>
#include <json.hpp>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
// Pool
#include "utility/ObjectPool/ObjectPool.h"

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
    void SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID,
        const Vector3& localOffset = {}, const std::string& parentBossName = "");
    void UIUpdate(const KetaEngine::ViewProjection& viewProjection);

    // 事前生成
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
    void LinkBossAndMinions(int32_t groupID, NormalEnemy* newMinion = nullptr, const std::string& parentBossName = "");
    void UpdateTauntState();
    void OnBossKilled(BaseEnemy* dyingBoss);

private:
    using json = nlohmann::json;

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "Enemies";

    std::array<BaseEnemy::Parameter, 2> parameters_;
    NormalEnemy::NormalParameter normalEnemyParam_;
    StrongEnemy::StrongParameter strongEnemyParam_;

#if defined(_DEBUG) || defined(DEVELOPMENT)
    // パラメータ変更検知用キャッシュ（変化した時だけ全敵にSetParameterを適用）
    std::array<BaseEnemy::Parameter, 2> cachedParameters_;
    NormalEnemy::NormalParameter cachedNormalEnemyParam_;
    StrongEnemy::StrongParameter cachedStrongEnemyParam_;
    bool parametersDirty_ = true;
#endif

    Player* pPlayer_;
    GameCamera* pGameCamera_;
    Combo* pCombo_             = nullptr;
    KillCounter* pKillCounter_ = nullptr;
    EnemySpawner* pEnemySpawner_;
    DeathTimer* pDeathTimer_                           = nullptr;
    const KetaEngine::ViewProjection* pViewProjection_ = nullptr;

    std::unique_ptr<EnemyDamageReactionController> damageReactionController_;

private:
    // 同時に存在しうる最大数
    static constexpr int32_t kNormalEnemyPoolSize = 48;
    static constexpr int32_t kStrongEnemyPoolSize = 8;

    ObjectPool<NormalEnemy> normalPool_;
    ObjectPool<StrongEnemy> strongPool_;

    std::vector<std::unique_ptr<BaseEnemy>> enemies_;
    std::vector<std::string> enemyTypes_ = {"NormalEnemy", "StrongEnemy"};

    std::unordered_map<int32_t, std::vector<std::unique_ptr<BaseEnemy>>> waitingEnemies_;
    std::unordered_map<BaseEnemy*, std::vector<NormalEnemy*>> minionsByBoss_;
    std::unordered_map<std::string, BaseEnemy*> bossByName_;
    std::unordered_map<BaseEnemy*, int32_t> bossGroupIndex_;
    int32_t nextBossGroupIndex_ = 0;

    std::string selectedEnemyType_;
    Vector3 spawnPosition_;

    bool areAllEnemiesCleared_;

    EnemyHPBarColorConfig hpBarColorConfig_;

    float hpBarDisplayDistance_;
    float uiOcclusionRadius_ = 80.0f;

    int32_t selectedEnemyTypeIndex_;

public:
    const bool& GetIsAllCleared() const { return areAllEnemiesCleared_; }
    const std::vector<std::unique_ptr<BaseEnemy>>& GetEnemies() const { return enemies_; }
    EnemyDamageReactionController* GetDamageReactionController() const { return damageReactionController_.get(); }
    float GetHpBarDisplayDistance() const { return hpBarDisplayDistance_; }
    float GetUIocclusionRadius() const { return uiOcclusionRadius_; }

    bool IsAnyEnemyInAnticipation() const;

    void SetPlayer(Player* player);
    void SetGameCamera(GameCamera* gameCamera);
    void SetCombo(Combo* combo);
    void SetKillCounter(KillCounter* killCounter);
    void SetEnemySpawner(EnemySpawner* enemySpawner);
    void SetDeathTimer(DeathTimer* deathTimer);
    void SetViewProjection(const KetaEngine::ViewProjection* vp) { pViewProjection_ = vp; }
};