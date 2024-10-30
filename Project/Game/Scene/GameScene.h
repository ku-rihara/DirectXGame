#pragma once

#include"base/Keta.h"
//DirectX
#include"base/WinApp.h"
#include "base/DirectXCommon.h"
#include"base/TextureManager.h"
//Transform
#include "3d/WorldTransform.h"
#include"3d/ViewProjection.h"
//obj
#include "3d/Mesh.h"
#include "3d/Model.h"
#include "2d/Sprite.h"
#include "audio/Audio.h"
#include"input/Input.h"
#include"3d/Object3d.h"
#include"3d/Object3dParticle.h"
//etc
#include"3d/DebugCamera.h"
#include<random>
#include <imgui.h>
#include<memory>
//struct 


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
	std::unique_ptr<Sprite> sprite2_ = nullptr;

	
	std::unique_ptr<Object3d> modelPlane_ = nullptr;
	std::unique_ptr<Object3d> modelFence_ = nullptr;
	std::unique_ptr<Object3d> modelSuzanne_ = nullptr;
	std::unique_ptr<Object3d> modelSuzanne2_ = nullptr;
	std::unique_ptr<Object3d> modelTerrain_ = nullptr;

	/// WorldTransform
	WorldTransform planeTransform_;
	WorldTransform fenceTransform_;
	WorldTransform suzanneTransform_;
	WorldTransform suzanneTransform2_;
	WorldTransform terrainTransform_;

	//パーティクル変数

	const uint32_t modelInstanceMax_ = 100;
	
	std::unique_ptr<Object3dParticle> modelPlaneParticle_ = nullptr;
	
	const float kDeltaTime_ = 1.0f / 60.0f;

	bool isDraw = true;

	ViewProjection viewProjection_;

	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	uint32_t uvHandle_;

	/*std::vector<std::unique_ptr<WorldTransform>>  planeTransforms_;*/
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

};