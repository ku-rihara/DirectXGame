#pragma once

#include "3d/ViewProjection.h"
#include "DamageReaction/EnemyDamageReactionController.h"
#include "Editor/ParticleEditor/ParticleEmitter.h"
#include "Types/BaseEnemy.h"

/// std
#include <json.hpp>
#include <list>
#include <memory>
#include <string>
#include <vector>

enum class Type;
struct BaseEnemy::Parameter;
class Player;
class LockOn;
class Combo;
class GameCamera;
class EnemySpawner;
class AttackEffect;

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
    void DamageEffectShot(const Vector3& pos);
    void ThrustEmit(const Vector3& pos);
    void DeathEmit(const Vector3& pos);
    void SpawnEmitByNormalEnemy(const Vector3& pos);
    void SpawnEmitByStrongEnemy(const Vector3& pos);
    void ParticleUpdate();
    
    // Param Edit
    void RegisterParams();
    void AdjustParam();
    void DamageReactionCreate();
    void DrawEnemyParamUI(BaseEnemy::Type type);

private: // struct
    struct ParticleEffect {
        std::string name;
        std::unique_ptr<KetaEngine::ParticleEmitter> emitter;
    };

private: // json
    using json = nlohmann::json;

private:
    // globalParameter
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "Enemies";
    std::array<BaseEnemy::Parameter, 2> parameters_;

    // ohter class
    Player* pPlayer_;
    GameCamera* pGameCamera_;
    Combo* pCombo_;
    EnemySpawner* pEnemySpawner_;
    AttackEffect* pAttackEffect_;

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

private: // Particle
    std::array<ParticleEffect, 4> deathParticle_;
    std::array<ParticleEffect, 1> debriParticle_;
    std::array<ParticleEffect, 4> damageEffect;
    std::array<ParticleEffect, 2> spawnEffectNormal_;
    std::array<ParticleEffect, 2> spawnEffectStrong_;
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
    void SetAttackEffect(AttackEffect* effect);
};
