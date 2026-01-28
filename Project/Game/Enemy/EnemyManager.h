#pragma once

// 3D
#include "3D/ViewProjection.h"
// DamageReaction
#include "DamageReaction/EnemyDamageReactionController.h"
// Particle
#include "Particle/CPUParticle/Editor/ParticleEmitter.h"
//BaseEnemy
#include "Types/BaseEnemy.h"

/// std
#include <json.hpp>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <array>

enum class Type;
struct BaseEnemy::Parameter;
class Player;
class LockOn;
class Combo;
class GameCamera;
class EnemySpawner;

class EnemyManager {
public:
    ///========================================================
    /// public method
    ///========================================================

    EnemyManager()  = default;
    ~EnemyManager() = default;

    // 初期化、更新
    void Init();
    void ParticleInit();
    void Update();

    // 敵全滅チェック
    void CheckIsEnemiesCleared();

    // 敵の生成
    void SpawnEnemy(const std::string& enemyType, const Vector3& position, int32_t groupID);
    void HpBarUpdate(const KetaEngine::ViewProjection& viewProjection);

    // Particle Effect
    void ThrustEmit(const Vector3& pos);
    void DeathEmit(const Vector3& pos);
    void ParticleUpdate();
    
    // Param Edit
    void RegisterParams();
    void AdjustParam();
    void DamageReactionCreate();
    void DrawEnemyParamUI(BaseEnemy::Type type);

    // debugSpawn
    void DebugEnemySpawn();

private: 
    // struct
    struct ParticleEffect {
        std::string name;
        std::unique_ptr<KetaEngine::ParticleEmitter> emitter;
    };

private: 
    // json
    using json = nlohmann::json;

private:
    // globalParameter
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "Enemies";

    // パラメータ
    std::array<BaseEnemy::Parameter, 2> parameters_;

    // ohter class
    Player* pPlayer_;
    GameCamera* pGameCamera_;
    Combo* pCombo_;
    EnemySpawner* pEnemySpawner_;
  
    // damageReaction
    std::unique_ptr<EnemyDamageReactionController> damageReactionController_;

private:
    ///========================================================
    /// Private variants
    ///========================================================

    // 敵リスト
    std::vector<std::unique_ptr<BaseEnemy>> enemies_;
    std::vector<std::string> enemyTypes_ = {"NormalEnemy", "StrongEnemy"};

    // 一時的な敵生成用データ
    std::string selectedEnemyType_;
    Vector3 spawnPosition_;

    // 敵がすべていなくなったことを示すフラグ
    bool areAllEnemiesCleared_;

    // debugSpawn用
    int32_t selectedEnemyTypeIndex_;

private: // Particle
    std::array<ParticleEffect, 4> deathParticle_;
    std::array<ParticleEffect, 1> debriParticle_;
    std::unique_ptr<KetaEngine::ParticleEmitter> fallCrack_;

public:
    ///========================================================
    /// getter method
    ///========================================================
    const bool& GetIsAllCleared() const { return areAllEnemiesCleared_; }
    const std::vector<std::unique_ptr<BaseEnemy>>& GetEnemies() const { return enemies_; }
    EnemyDamageReactionController* GetDamageReactionController() const { return damageReactionController_.get(); }
    ///========================================================
    /// setter method
    ///========================================================
    void SetPlayer(Player* plyaer);
    void SetLockOn(LockOn* lockOn);
    void SetGameCamera(GameCamera* gameCamera);
    void SetCombo(Combo* combo);
    void SetEnemySpawner(EnemySpawner* enemySpawner);
};
