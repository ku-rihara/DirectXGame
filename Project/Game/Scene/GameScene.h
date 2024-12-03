#pragma once

//DirectX
#include"base/WinApp.h"
#include "base/DirectXCommon.h"
#include"base/TextureManager.h"
//Transform
#include "3d/WorldTransform.h"
#include"3d/ViewProjection.h"
//obj
#include "2d/Sprite.h"
#include "audio/Audio.h"
#include"input/Input.h"
#include"3d/Object3d.h"
//etc
#include"utility/Debug/DebugCamera.h"
#include<memory>
//testobj
#include"TestObj/CollisionTest1.h"
#include"TestObj/CollisionTest2.h"
// emitter
#include"utility/Particle/ParticleEmitter.h"


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

	/// particle
	std::unique_ptr<ParticleEmitter>emitter_;
	float time_;

	bool isDraw = true;

	ViewProjection viewProjection_;

	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	uint32_t circleHandle_;
	uint32_t uv_;

	// test
	std::unique_ptr<CollisionTest1>collisionTest1_;
	std::unique_ptr<CollisionTest2>collisionTest2_;
	
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


	const ViewProjection& GetViewProjection()const { return viewProjection_; }
};