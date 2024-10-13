#pragma once

#include"Keta.h"
//DirectX
#include "DirectXCommon.h"
#include"TextureManager.h"
//2d
#include "Sprite.h"
#include "Audio.h"
//3d
#include "Model.h"
#include"Object3d.h"
//camera
#include"DebugCamera.h"
#include"GameCamera.h"
//obj
#include"RailManager.h"
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
	//デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
	//ゲームカメラ
	std::unique_ptr<GameCamera>gameCamera_ = nullptr;
	//レールマネージャー
	std::unique_ptr<RailManager>railManager_;

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

};