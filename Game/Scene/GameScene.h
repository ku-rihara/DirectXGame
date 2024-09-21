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
	uint32_t modelInstance_ = 0;

	bool isDrawPlane_ = true;

	ViewProjection viewProjection_;
	WorldTransform planeTransform_;
	WorldTransform fenceTransform_;
	WorldTransform transformSprite_;
	WorldTransform uvTransformSprite_;
	std::vector<std::unique_ptr<WorldTransform>>  planeTransforms_;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
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

};