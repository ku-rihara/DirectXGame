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
    putObjForBlender     = std::make_unique<PutObjForBlender>();
    timeScaleController_ = std::make_unique<TimeScaleController>();
    cameraPlayer_        = std::make_unique<CameraAnimation>();

    /*  monsterBall_->Init();*/
   
    skuBox_->Init();
    timeScaleController_->Init();

    cameraPlayer_->Init();
    cameraPlayer_->SetViewProjection(&viewProjection_);

    putObjForBlender->LoadJsonFile("game.json");
    putObjForBlender->EasingAllReset();

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

    // 各クラス更新
  
    timeScaleController_->Update(Frame::DeltaTime());

  
    skuBox_->Update();

    putObjForBlender->EasingUpdateSelectGroup(Frame::DeltaTime(), 0);

    // viewProjection 更新
    ViewProjectionUpdate();

    if (input_->TriggerKey(KeyboardKey::Enter)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
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
    Light::GetInstance()->DebugImGui();
    ImGui::Begin("Object");
    skuBox_->Debug();
    ShadowMap::GetInstance()->DebugImGui();
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
