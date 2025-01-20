#pragma once

///baseScene
#include"BaseScene.h"
#include"utility/Particle/ParticleEmitter.h"

#include"Player/Player.h"
#include"Field/Field.h"
#include"SkyDome/SkyDome.h"
#include"UI/TitleFont.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene:public BaseScene {
private:
	

	int shandle_;
	std::unique_ptr<Sprite>screenSprite_;

	std::unique_ptr<Player>player_ = nullptr;
	std::unique_ptr<Field>field_ = nullptr;
	std::unique_ptr<Skydome> skydome_ = nullptr;
	std::unique_ptr<TitleFont>titleFont_ = nullptr;

	bool isFontUpdate_;
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