#pragma once

#include"base/Keta.h"
//DirectX
#include "base/DirectXCommon.h"
#include"base/TextureManager.h"
//2d
#include "2d/Sprite.h"
#include "audio/Audio.h"
//3d
#include "3d/Model.h"
#include"3d/Object3d.h"
//camera
#include"3d/DebugCamera.h"
#include"Camera/GameCamera.h"
//obj
#include"ControlPoint/ControlPointManager.h"
#include"Enemy/EnemyManager.h"
#include"Player/Player.h"
#include"Reticle/Reticle.h"
#include"skyDome/SkyDome.h"
#include"PositionEditor/PositionEditor.h"
#include"Score/Score.h"
//std
#include<random>

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
	//ビュープロジェクション
	ViewProjection viewProjection_;
	
////////////////////////////////////////////////////////////////////////////////////////////
//  obj
////////////////////////////////////////////////////////////////////////////////////////////
	/// デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
	/// ゲームカメラ
	std::unique_ptr<GameCamera>gameCamera_ = nullptr;
	/// レールマネージャー
	std::unique_ptr<ControlPointManager>controlPointManager_=nullptr;
	/// エネミーマネージャー
	std::unique_ptr<EnemyManager>enemyManager_ = nullptr;
	/// プレイや
	std::unique_ptr<Player>player_ = nullptr;
	/// レティクル
	std::unique_ptr<Reticle>reticle_=nullptr;
	/// 天球
	std::unique_ptr<Skydome>skyDome_ = nullptr;
	/// 配置エディター
	std::unique_ptr<PositionEditor>positionEditor_ = nullptr;
	/// スコア
	std::unique_ptr<Score>score_ = nullptr;


	// デバッグカメラアクティブ
	bool isDebugCameraActive_ = false;
public: 
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
	void ModelDraw();

	void SpriteDraw();

	void ParticleDraw();

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// ビュープロジェクション更新
	/// </summary>
	void ViewProjectionUpdate();


////////////////////////////////////////////////////////////////////////////////////////////
//  getter
////////////////////////////////////////////////////////////////////////////////////////////
	const ViewProjection& GetViewProjection()const { return viewProjection_; }

};