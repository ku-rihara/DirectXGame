/// scene
#include "TitleScene.h"
#include "3d/Object3DRegistry.h"
#include"2d/SpriteRegistry.h"
#include "Lighrt/Light.h"
#include "Scene/Manager/SceneManager.h"
#include "Player/TitleBehavior/TitleRightPunch.h"
#include "Editor/ParticleEditor/ParticleManager.h"

#include "base/TextureManager.h"
#include "Frame/Frame.h"

// math
#include <imgui.h>


TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
}

void TitleScene::Init() {
    //// グローバル変数の読み込み
    KetaEngine::GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();
    ///=======================================================================================
    /// 生成
    ///=======================================================================================

    field_            = std::make_unique<Field>();
    player_           = std::make_unique<Player>();
    skydome_          = std::make_unique<SkyDome>();
    titleLogo_        = std::make_unique<TitleLogo>();
    skyBox_           = std::make_unique<SkyBox>();
    ObjectFromBlender_ = std::make_unique<KetaEngine::ObjectFromBlender>();

    ///=======================================================================================
    /// 初期化
    ///=======================================================================================
    field_->Init();
    skyBox_->Init();
    player_->Init();
    titleLogo_->Init();
    ObjectFromBlender_->LoadJsonFile("gameScene.json");

    alpha_ = 0.0f;

    viewProjection_.translation_ = {7.8f, 3.6f, 8.3f};
    viewProjection_.rotation_.y  = 3.8f;

    screenSprite_.reset(KetaEngine::Sprite::Create("screenChange.png"));

    player_->SetTitleBehavior();
    player_->SetWorldPositionY(30.0f);

    KetaEngine::ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);

}

void TitleScene::Update() {

    BaseScene::Update();

    screenSprite_->SetAlpha(alpha_);

    player_->TitleUpdate();
    field_->Update();
    skyBox_->Update();

    if (dynamic_cast<TitleRightPunch*>(player_->GetTitleBehavior())) {
        if (!isTitleLogoUpdate) {
            isTitleLogoUpdate = true;
        }
    }
    if (isTitleLogoUpdate) {
        titleLogo_->Update();
    }

  
    // 　ゲーム遷移
    if (KetaEngine::Input::GetInstance()->PushKey(KeyboardKey::Space)) {
        isFinished_ = true;
    } else {
        ChangeForJoyState(); // コントローラジャンプ
    }

 
    ViewProjectionUpdate();

    if (isFinished_) {
        alpha_ += KetaEngine::Frame::DeltaTime();
        if (alpha_ >= 1.2f) {
            KetaEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
        }
    }
}

void TitleScene::ChangeForJoyState() {

    if (!((KetaEngine::Input::IsTriggerPad(0, GamepadButton::A)))) {
        return;
    }

    isFinished_ = true;
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void TitleScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void TitleScene::Debug() {
#ifdef _DEBUG
 

    ImGui::Begin("Param");
    KetaEngine::Light::GetInstance()->DebugImGui();
    KetaEngine::SpriteRegistry::GetInstance()->DebugImGui();
    ImGui::End();
#endif
}

// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
    KetaEngine::Light::GetInstance()->SetWorldCameraPos(player_->GetWorldPosition());
}

void TitleScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}