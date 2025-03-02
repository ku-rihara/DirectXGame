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
	std::string smokeName_;
	std::unique_ptr<ParticleEmitter>smokeEmitter_;

	std::string fireSmokeName_;
	std::unique_ptr<ParticleEmitter>fireSmokeEmitter_;

	std::string sparkName_;
	std::unique_ptr<ParticleEmitter>sparkEmitter_;

	std::string miniSparkName_;
	std::unique_ptr<ParticleEmitter>miniSparkEmitter_;

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