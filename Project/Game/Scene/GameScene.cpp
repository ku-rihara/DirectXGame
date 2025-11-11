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

    ground_              = std::make_unique<Ground>();
    monsterBall_         = std::make_unique<MonsterBall>();
    plane_               = std::make_unique<Plane>();
    skuBox_              = std::make_unique<SkyBox>();
    putObjForBlender     = std::make_unique<PutObjForBlender>();
    timeScaleController_ = std::make_unique<TimeScaleController>();

  /*  monsterBall_->Init();*/
    ground_->Init();
    plane_->Init();
    skuBox_->Init();
    timeScaleController_->Init();

   putObjForBlender->LoadJsonFile("game.json");
    putObjForBlender->EasingAllReset();

    cameraMode_ = BaseScene::CameraMode::DEBUG;

    testGpuParticleEmitter_.reset(GPUParticleEmitter::CreateParticlePrimitive("test", PrimitiveType::Plane, 1024));
}

void GameScene::Update() {

    /// debugCamera
    BaseScene::Update();
  
    // 各クラス更新
    ground_->Update();
    timeScaleController_->Update(Frame::DeltaTime());
 
    plane_->Update();
    skuBox_->Update();

    testGpuParticleEmitter_->Emit();
    testGpuParticleEmitter_->Update();

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

    Light::GetInstance()->DebugImGui();
    ImGui::Begin("Object");
    ground_->Debug();
    monsterBall_->Debug();
    plane_->Debug();
    skuBox_->Debug();
    ShadowMap::GetInstance()->DebugImGui();

    ImGui::End();
    cameraEditor_->EditorUpdate();
    shakeEditor_->EditorUpdate();
    railEditor_->EditorUpdate();
    objEaseAnimationEditor_->EditorUpdate();
    timeScaleController_->EditorUpdate();
#endif
}

// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}
