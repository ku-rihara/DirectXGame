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

    skuBox_           = std::make_unique<SkyBox>();
    ObjectFromBlender = std::make_unique<KetaEngine::ObjectFromBlender>();
    cameraPlayer_     = std::make_unique<KetaEngine::CameraAnimation>();
    timeLine_         = std::make_unique<KetaEngine::TimeLine>();

    /*  monsterBall_->Init();*/

    skuBox_->Init();
    timeLine_->Init();

      // トラック追加
    uint32_t posXTrack = timeLine_->AddTrack("Camera Pos X", [&](float value) {
        viewProjection_.translation_.x = value;
    });

    uint32_t posYTrack = timeLine_->AddTrack("Camera Pos Y", [&](float value) {
     
        viewProjection_.translation_.y = value;
    });

    // キーフレーム追加
    timeLine_->AddKeyFrame(posXTrack, 0, 0.0f, 5.0f);
    timeLine_->AddKeyFrame(posXTrack, 30, 10.0f, 10.0f);
    timeLine_->AddKeyFrame(posYTrack, 0, 0.0f, 3.0f);
    timeLine_->AddKeyFrame(posYTrack, 20, 5.0f, 8.0f);

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

     timeLine_->Draw();
#endif
}

// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}
