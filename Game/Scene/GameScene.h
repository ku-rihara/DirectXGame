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
#include"Object3d.h"
//etc
#include"DebugCamera.h"
#include<random>
#include <imgui.h>
#include<memory>

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

	std::unique_ptr<Object3d> modelPlane_ = nullptr;
	std::unique_ptr<Object3d> modelFence_ = nullptr;
	std::unique_ptr<Object3d> modelSuzanne_ = nullptr;
	std::unique_ptr<Object3d> modelTerrain_ = nullptr;
	
	//パーティクル変数

	const uint32_t modelInstanceMax_ = 10;
	std::vector<Vector4>particleColor_;
	std::unique_ptr<Model> modelPlaneParticle_ = nullptr;
	std::vector<Vector3> particleVelocity_;
	const float kDeltaTime_ = 1.0f / 60.0f;

	bool isDraw = true;

	ViewProjection viewProjection_;
	WorldTransform planeTransform_;
	WorldTransform fenceTransform_;
	WorldTransform suzanneTransform_;
	WorldTransform terrainTransform_;
	WorldTransform transformSprite_;
	WorldTransform uvTransformSprite_;
	std::vector<std::unique_ptr<WorldTransform>>  planeTransforms_;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	uint32_t uvHandle_;
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

	//パーティクル関連
	void MakeParticle(std::mt19937& randomEngine);
};