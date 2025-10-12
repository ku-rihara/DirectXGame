#include "GameScene.h"
#include "base/TextureManager.h"
// class
#include "3d/Object3DRegistry.h"
#include "utility/ParticleEditor/ParticleManager.h"
#include "GPUParticle/GPUParticleManager.h"

// math
#include "Frame/Frame.h"
#include "Lighrt/Light.h"
#include "Scene/Manager/SceneManager.h"

#include "Animation/AnimationRegistry.h"
#include "Pipeline/PipelineManager.h"
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
    cameraEditor_        = std::make_unique<CameraEditor>();
    shakeEditor_         = std::make_unique<ShakeEditor>();
    timeScaleController_ = std::make_unique<TimeScaleController>();

    monsterBall_->Init();
    ground_->Init();
    plane_->Init();
    skuBox_->Init();
    cameraEditor_->Init(&viewProjection_);
    timeScaleController_->Init();

    shakeEditor_->Init();
    putObjForBlender->LoadJsonFile("game.json");
    putObjForBlender->EasingAllReset();

    isDebugCameraActive_ = true;

    testGpuParticleEmitter_.reset(GPUParticleEmitter::CreateParticlePrimitive("test", PrimitiveType::Box, 1024));

    ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
    GPUParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
}

void GameScene::Update() {

    /// debugCamera
    debugCamera_->Update();
    Debug();

    // 各クラス更新
    ground_->Update();
    cameraEditor_->Update();
    shakeEditor_->Update(Frame::DeltaTime());
    timeScaleController_->Update(Frame::DeltaTime());
    monsterBall_->Update();
    plane_->Update();
    skuBox_->Update();

    Object3DRegistry::GetInstance()->UpdateAll();
    AnimationRegistry::GetInstance()->UpdateAll(Frame::DeltaTimeRate());
    testGpuParticleEmitter_->Emit();
    testGpuParticleEmitter_->Update();

    putObjForBlender->EasingUpdateSelectGroup(Frame::DeltaTime(), 0);

    // viewProjection 更新
    ViewProjectionUpdate();

    if (input_->TriggerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }

    ParticleManager::GetInstance()->Update();
    GPUParticleManager::GetInstance()->Update();
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
    timeScaleController_->EditorUpdate();
#endif
}

// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProssess() {
    viewProjection_.UpdateMatrix();
}
