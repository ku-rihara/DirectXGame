#include "GameScene.h"

// math
#include "Frame/Frame.h"
#include "Lighrt/Light.h"
#include "Scene/Manager/SceneManager.h"

#include "ShadowMap/ShadowMap.h"

#include <imgui.h>

GameScene::GameScene() {}

GameScene::~GameScene() {
}

void GameScene::Init() {

    BaseScene::Init();

    skuBox_              = std::make_unique<SkyBox>();
    ObjectFromBlender     = std::make_unique<KetaEngine::ObjectFromBlender>();
   cameraPlayer_        = std::make_unique<KetaEngine::CameraAnimation>();

    /*  monsterBall_->Init();*/

    skuBox_->Init();
 
    cameraPlayer_->Init();
    cameraPlayer_->SetViewProjection(&viewProjection_);

    ObjectFromBlender->LoadJsonFile("game.json");
    ObjectFromBlender->EasingAllReset();

    cameraMode_ = BaseScene::CameraMode::DEBUG;
}

void GameScene::Update() {

    /// debugCamera
    BaseScene::Update();

    if (input_->TriggerKey(KeyboardKey::R)) {
        cameraPlayer_->Play("PlayerthrustAttack");
    }

    if (input_->TriggerKey(KeyboardKey::T)) {
        cameraPlayer_->Play("test2");
    }

    cameraPlayer_->Update();

    skuBox_->Update();

    ObjectFromBlender->EasingUpdateSelectGroup(KetaEngine::Frame::DeltaTime(), 0);

    // viewProjection 更新
    ViewProjectionUpdate();

    if (input_->TriggerKey(KeyboardKey::Enter)) {
        KetaEngine::SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void GameScene::SkyBoxDraw() {
    skuBox_->Draw(viewProjection_);
}

void GameScene::Debug() {
#ifdef _DEBUG
    BaseScene::Debug();
    KetaEngine::Light::GetInstance()->DebugImGui();
    ImGui::Begin("Object");
    skuBox_->Debug();
    KetaEngine::ShadowMap::GetInstance()->DebugImGui();
    ImGui::End();

    ImGui::Begin("editor");
    timeScaleController_->EditorUpdate();
    ImGui::End();
#endif
}

// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}
