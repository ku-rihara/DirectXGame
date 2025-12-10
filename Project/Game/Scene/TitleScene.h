#pragma once

///baseScene
#include"BaseScene.h"


#include"Editor/ParticleEditor/ParticleEmitter.h"
#include<string>
#include<array>

/// <summary>
///  タイトルシーン
/// </summary>
class TitleScene :public BaseScene {
private:
	
	std::array<std::unique_ptr<ParticleEmitter>,3>EnemydamageEffect_;
    std::array<std::unique_ptr<ParticleEmitter>, 1> afterGlowEffect_;
  	
public:
	
	TitleScene();
	~TitleScene()override;

	///========================================================
	/// private method
	///========================================================

	/// 初期化、更新、描画
	void Init()override;
	void Update()override;
    void SkyBoxDraw() override;


	void Debug()override;/// debug
	void ViewProjectionUpdate()override;
	void ViewProcess()override;



};