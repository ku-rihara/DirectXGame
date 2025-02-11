#pragma once

///baseScene
#include"BaseScene.h"

#include"TestObj/CollisionTest1.h"
#include"TestObj/CollisionTest2.h"
#include"TestObj/CollisionTest3.h"
#include"utility/Particle/ParticleEmitter.h"
/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene:public BaseScene {
private:

	std::unique_ptr<ParticleEmitter>emitter_;
	

public:
	///========================================================
	/// Constralt destract
	///========================================================
	TitleScene();
	~TitleScene()override;

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



};