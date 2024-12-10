#pragma once

/// base
#include"BaseScene.h"

//testobj
#include"TestObj/CollisionTest1.h"
#include"TestObj/CollisionTest2.h"
#include"Ground/Ground.h"
// emitter
#include"utility/Particle/ParticleEmitter.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene:public BaseScene {
private:

	///========================================================
	/// Private variants
	///========================================================

	uint32_t textureHandle_ = 0;
	uint32_t soundDataHandle_ = 0;
	uint32_t voiceHandle_ = 0;
	

	std::unique_ptr<Ground> ground_ = nullptr;
	
	
	//bool isDebugCameraActive_;
	bool isDraw = true;

	/*ViewProjection viewProjection_;*/

	/*std::unique_ptr<DebugCamera> debugCamera_ = nullptr;*/

	uint32_t defaultHandle_;
	uint32_t circleHandle_;
	uint32_t uv_;

	

public:
	
	///========================================================
	/// Constralt destract
	///========================================================
	GameScene();
	~GameScene()override;

	///========================================================
	/// private method
	///========================================================
	
	/// 初期化、更新、描画
	void Init()override;
	void Update()override;
	void ModelDraw()override;
	void SpriteDraw()override;
	void ParticleDraw()override;

	void Debug()override;/// debug
	void ViewProjectionUpdate()override;

	/*const ViewProjection& GetViewProjection()const { return viewProjection_; }*/
};