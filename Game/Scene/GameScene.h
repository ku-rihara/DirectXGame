#pragma once

#include"Keta.h"
//DirectX
#include"WinApp.h"
#include "DirectXCommon.h"
#include"TextureManager.h"
//Transform
#include "WorldTransform.h"
#include"ViewProjection.h"
//obj
#include "Mesh.h"
#include "Model.h"
#include "Sprite.h"
#include "Audio.h"
#include"Input.h"
//etc
#include"DebugCamera.h"
#include <imgui.h>
#include<memory>

//class
#include "player.h"
#include "Enemy.h"
#include "SkyDome.h"
#include "Ground.h"
#include "FollowCamera.h"
#include "CollisionManager.h"
#include"LockOn.h"
#include "Hummer.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {
private:

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	TextureManager* textureManager_ = nullptr;

	uint32_t textureHandle_ = 0; 
	uint32_t soundDataHandle_ = 0; 
	uint32_t voiceHandle_ = 0;     
	std::unique_ptr<Sprite> sprite_ = nullptr;

	std::unique_ptr<Model> modelPlane_ = nullptr;
	std::unique_ptr<Model> modelFence_ = nullptr;
	std::unique_ptr<Model> modelSuzanne_ = nullptr;
	std::unique_ptr<Model> modelTerrain_ = nullptr;

	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
	std::unique_ptr<Model> modelSkyDome_ = nullptr; // 3Dモデル
	std::unique_ptr<Model> modelGround_ = nullptr; // 3Dモデル
	//プレイヤーモデル
	std::unique_ptr<Model> modelFighterBody_ = nullptr;
	std::unique_ptr<Model> modelFighterHead_ = nullptr;
	std::unique_ptr<Model> modelFighterLeftArm_ = nullptr;
	std::unique_ptr<Model> modelFighterRightArm_ = nullptr;
	//武器モデル
	std::unique_ptr<Model> modelPlayerWeapon_ = nullptr;
	//敵モデル
	std::unique_ptr<Model> modelEnemyBody_ = nullptr;
	std::unique_ptr<Model> modelEnemyThurn_ = nullptr;

	std::unique_ptr<Player> player_;
	std::unique_ptr<Hummer> hummer_;
	std::unique_ptr<Enemy> enemy_;
	/*std::unique_ptr<Effect> effect_;*/
	std::unique_ptr<Skydome> skyDome_;
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<CollisionManager> collisionManager_;

	//敵リスト
	std::list<std::unique_ptr<Enemy>> enemies_;
	//ロックオン
	std::unique_ptr<LockOn> lockOn_;
	


	uint32_t modelInstance_ = 0;

	bool isDrawPlane_ = true;

	bool isDebugCameraActive_ = false;


	ViewProjection viewProjection_;
	WorldTransform worldTransform_;
	
	std::vector<std::unique_ptr<WorldTransform>>  planeTransforms_;

	uint32_t uvHandle_;
public: 
	// エフェクトモデル
	static std::unique_ptr<Model> modelEffect_;
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void AddEnemy();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();
};