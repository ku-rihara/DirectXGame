#pragma once

///baseScene
#include"BaseScene.h"

#include"Plane/Plane.h"
#include"utility/ParticleEditor/ParticleEmitter.h"
#include<string>
#include<array>
/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene :public BaseScene {
private:
	
	std::array<std::unique_ptr<ParticleEmitter>,3>EnemydamageEffect_;
    std::unique_ptr<Plane> plane_ = nullptr;
	
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
    void SkyBoxDraw() override;

	void Debug()override;/// debug
	void ViewProjectionUpdate()override;
	void ViewProssess()override;



};