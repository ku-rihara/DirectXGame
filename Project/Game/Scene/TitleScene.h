#pragma once

///baseScene
#include"BaseScene.h"


#include"utility/Particle/ParticleEmitter.h"
#include<string>
/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene :public BaseScene {
private:
	
	std::string debriName_;
	std::unique_ptr<ParticleEmitter>debriEmitter_;

	std::string starName_;
	std::unique_ptr<ParticleEmitter>starEmitter_;

	std::string centerStar_;
	std::unique_ptr<ParticleEmitter>centerStarEmitter_;

	std::string starFrame_;
	std::unique_ptr<ParticleEmitter>starFrameEmitter_;
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
	/*void ParticleDraw()override;*/

	void Debug()override;/// debug
	void ViewProjectionUpdate()override;
	void ViewProssess()override;



};