#pragma once

/// base
#include"BaseScene.h"

//object
#include"Field/Field.h"
#include"SkyDome/SkyDome.h"
#include"Player/Player.h"
#include"LockOn/LockOn.h"
#include"GameCamera/GameCamera.h"
#include"Enemy/EnemyManager.h"




/// <summary>
/// ゲームシーン
/// </summary>
class GameScene:public BaseScene {
private:

	///========================================================
	/// Private variants
	///========================================================

	
	///* objects
	std::unique_ptr<GameCamera> gamecamera_ = nullptr;
	std::unique_ptr<Field> field_ = nullptr;
	std::unique_ptr<LockOn> lockOn_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;
	std::unique_ptr<Skydome> skydome_ = nullptr;
	std::unique_ptr<EnemyManager>enemyManager_ = nullptr;
	
	bool isDraw = true;

	

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
	void ViewProssess()override;

	/*const ViewProjection& GetViewProjection()const { return viewProjection_; }*/
};