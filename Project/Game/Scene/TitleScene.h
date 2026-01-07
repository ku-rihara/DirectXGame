#pragma once

///baseScene
#include"BaseScene.h"
#include"Editor/ParticleEditor/ParticleEmitter.h"

#include"Player/Player.h"
#include"Field/Field.h"
#include"SkyDome/SkyDome.h"
#include"SkyBox/SkyBox.h"
#include"UI/TitleLogo.h"
#include "utility/ObjectFromBlender/ObjectFromBlender.h"

/// <summary>
///  タイトルシーン
/// </summary>
class TitleScene : public BaseScene {
private:
	

	int shandle_;
    std::unique_ptr<KetaEngine::Sprite> screenSprite_;

	std::unique_ptr<Player>player_ = nullptr;
	std::unique_ptr<Field>field_ = nullptr;
	std::unique_ptr<SkyDome> skydome_ = nullptr;
    std::unique_ptr<SkyBox> skyBox_      = nullptr;
    std::unique_ptr<TitleLogo> titleLogo_ = nullptr;
    std::unique_ptr<KetaEngine::ObjectFromBlender> ObjectFromBlender_ = nullptr;

	bool isTitleLogoUpdate;
	bool isFinished_;
	float alpha_;
public:
    TitleScene();
    ~TitleScene() override;

    ///========================================================
    /// private method
    ///========================================================

    /// 初期化、更新、描画
    void Init() override;
    void Update() override;
    void SkyBoxDraw() override;


	void ChangeForJoyState();

	void Debug()override;/// debug
	void ViewProjectionUpdate()override;
	void ViewProcess()override;



};