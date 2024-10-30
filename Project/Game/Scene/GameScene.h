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
#include"Rail/RailManager.h"
#include"Enemy/EnemyManager.h"
#include"Player/Player.h"
#include"Reticle/Reticle.h"
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
	//3Dマウス位置可視化
	std::unique_ptr<Object3d>mousePosView_;
	WorldTransform mousePosTransform_;
////////////////////////////////////////////////////////////////////////////////////////////
//  obj
////////////////////////////////////////////////////////////////////////////////////////////
	/// デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
	/// ゲームカメラ
	std::unique_ptr<GameCamera>gameCamera_ = nullptr;
	/// レールマネージャー
	std::unique_ptr<RailManager>railManager_=nullptr;
	/// エネミーマネージャー
	std::unique_ptr<EnemyManager>enemyManager_ = nullptr;
	/// プレイや
	std::unique_ptr<Player>player_ = nullptr;
	/// レティクル
	std::unique_ptr<Reticle>reticle_=nullptr;

	//変数
const float mouseDepth_=0.995f;
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
	void Draw();

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