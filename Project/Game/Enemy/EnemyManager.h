#pragma once

#include "BaseEnemy.h"
#include "3d/ViewProjection.h"
#include"utility/ParticleEditor/ParticleEmitter.h"


/// std
#include <vector>
#include <list>
#include <memory>
#include <string>
#include<map>
#include <json.hpp>

enum class Type;
struct BaseEnemy::Paramater;
class Player;
class LockOn;
class Combo;
class GameCamera;
class EnemySpawner;
class EnemyManager {
private:
	struct ParticleEffect {
		std::string name;
		std::unique_ptr<ParticleEmitter>emitter;
	};
private:
	using json = nlohmann::json;

	///========================================================
	/// Private variants
	///========================================================

    /// グローバルなパラメータ
	GlobalParameter* globalParameter_;            
	const std::string groupName_ = "Enemies";     
	std::array<BaseEnemy::Paramater,2> paramaters_;

	// ohter class
	Player* pPlayer_;
	LockOn* pLockOn_;
	GameCamera* pGameCamera_;
    Combo* pCombo_;
    EnemySpawner* pEnemySpawner_;

	///* 敵リスト
	std::list<std::unique_ptr<BaseEnemy>> enemies_;

	///* 敵の種類リスト
	std::vector<std::string> enemyTypes_ = { "NormalEnemy","StrongEnemy" };

	//* 一時的な敵生成用データ
	std::string selectedEnemyType_;
	Vector3 spownPosition_;
	uint32_t spownNum_;

	bool areAllEnemiesCleared_; // 敵がすべていなくなったことを示すフラグ

private:
	/// Particle
	std::array<ParticleEffect, 4>deathParticle_;
	std::array<ParticleEffect, 1>debriParticle_;
	std::array<ParticleEffect, 4>damageEffect;
    std::array<ParticleEffect, 2> spawnEffectNormal_;
    std::array<ParticleEffect, 2> spawnEffectStrong_;
	std::unique_ptr<ParticleEmitter>fallCrack_;
public:

	///========================================================
	/// public method
	///========================================================

	// コンストラクタ
	EnemyManager()=default;

	// 初期化
	void Init();
	void FSpawn();
	void ParticleInit();

	void GetIsEnemiesCleared();

	// 敵の生成
    void SpawnEnemy(const std::string& enemyType, const Vector3& position, const int32_t& groupID);

	// 更新処理
	void Update();
	void HpBarUpdate(const ViewProjection& viewProjection);
	
	// 描画処理
	void Draw(const ViewProjection& viewProjection);

	// スプライト描画処理
	void SpriteDraw(const ViewProjection& viewProjection);

	///-------------------------------------------------------------------------------------
	///Particle
	///-------------------------------------------------------------------------------------
	void DamageEffectShot(const Vector3& pos);
	void ThrustEmit(const Vector3& pos);
	void DeathEmit(const Vector3& pos);
    void SpawnEmitByNormalEnemy(const Vector3& pos);
    void SpawnEmitByStrongEnemy(const Vector3& pos);
	void ParticleUpdate();
	///-------------------------------------------------------------------------------------
	///Editor
	///-------------------------------------------------------------------------------------
	void ParamLoadForImGui();
	void AddParamGroup();
	void SetValues();
	void ApplyGlobalParameter();
	void AdjustParam();
    void DrawEnemyParamUI(BaseEnemy::Type type);

	///========================================================
   /// getter method
   ///========================================================
	bool GetCread()const { return areAllEnemiesCleared_; }
	///========================================================
	/// setter method
	///========================================================
	void SetPlayer(Player* plyaer);
	void SetLockon(LockOn* lockOn);
	void SetGameCamera(GameCamera* gamecamera);
    void SetCombo(Combo* combo);
    void SetEnemySpawner(EnemySpawner* enemySpawner);

	// フェーズの切り替え
	void SetPhase(int phase);

	// 現在の敵リスト取得
	const std::list<std::unique_ptr<BaseEnemy>>& GetEnemies() const {
		return enemies_;
	}
};

