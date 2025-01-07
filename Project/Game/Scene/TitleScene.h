#pragma once

///baseScene
#include"BaseScene.h"

#include"utility/Particle/ParticleEmitter.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene:public BaseScene {
private:
	/*std::string damageName_;
	std::unique_ptr<ParticleEmitter>damageEmitter_;*/
	int handle_;
	std::unique_ptr<Sprite>titleSprite_;

	int shandle_;
	std::unique_ptr<Sprite>screenSprite_;

	bool isFinished_;
	float alpha_;
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

	void ChangeForJoyState();

	void Debug()override;/// debug
	void ViewProjectionUpdate()override;
	void ViewProssess()override;



};