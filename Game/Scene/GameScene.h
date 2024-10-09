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
#include"Object3dParticle.h"
//etc
#include"DebugCamera.h"
#include<random>
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
	
	ViewProjection viewProjection_;

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

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

};