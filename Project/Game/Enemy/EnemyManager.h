#pragma once

#include "BaseEnemy.h"
#include "3d/ViewProjection.h"

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
class GameCamera;
class EnemyManager {
private:
	using json = nlohmann::json;

	///========================================================
	/// Private variants
	///========================================================

    /// グローバルなパラメータ
	GlobalParameter* globalParameter_;            /// グローバルパラメータ
	const std::string groupName_ = "Enemies";      /// グループ名
	std::array<BaseEnemy::Paramater,2> paramaters_;

	// ohter class
	Player* pPlayer_;
	LockOn* pLockOn_;
	GameCamera* pGameCamera_;

	///* 敵リスト
	std::list<std::unique_ptr<BaseEnemy>> enemies_;

	///* 敵の種類リスト
	std::vector<std::string> enemyTypes_ = { "NormalEnemy","StrongEnemy" };

	//* 一時的な敵生成用データ
	std::string selectedEnemyType_;
	Vector3 spownPosition_;
	uint32_t spownNum_;

	bool areAllEnemiesCleared_; // 敵がすべていなくなったことを示すフラグ

public:

	///========================================================
	/// public method
	///========================================================

	// コンストラクタ
	EnemyManager();

	// 初期化
	void Init();
	void FSpawn();
	void ParticleInit();

	void GetIsEnemiesCleared();

	// 敵の生成
	void SpawnEnemy(const std::string& enemyType, const Vector3& position);

	// 更新処理
	void Update();
	void HpBarUpdate(const ViewProjection& viewProjection);
	
	// 描画処理
	void Draw(const ViewProjection& viewProjection);

	// スプライト描画処理
	void SpriteDraw(const ViewProjection& viewProjection);

	///-------------------------------------------------------------------------------------
	///Editor
	///-------------------------------------------------------------------------------------
	void ParmLoadForImGui();
	void AddParmGroup();
	void SetValues();
	void ApplyGlobalParameter();
	void AdjustParm();

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

	// フェーズの切り替え
	void SetPhase(int phase);

	// 現在の敵リスト取得
	const std::list<std::unique_ptr<BaseEnemy>>& GetEnemies() const {
		return enemies_;
	}
};

