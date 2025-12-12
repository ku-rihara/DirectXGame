/// scene
#include "EditorScene.h"
#include "GameScene.h"
#include "Scene/Manager/SceneManager.h"

#include "base/TextureManager.h"
// class
#include "3d/Object3DRegistry.h"
#include "Editor/ParticleEditor/ParticleManager.h"
#include "GPUParticle/GPUParticleManager.h"
#include "Pipeline/PipelineManager.h"

// math
#include "Frame/Frame.h"
#include <imgui.h>

EditorScene::EditorScene() {}

EditorScene::~EditorScene() {
}

void EditorScene::Init() {

    BaseScene::Init();

    easingTestObject_ = std::make_unique<EasingTestObj>();
    easingTestObject_->Init();

    easingEditor_.Init();
    easingEditor_.SetVector3Target(&easingTestObject_->GetEasingData());

   
}

void EditorScene::Update() {

    BaseScene::Update();

    easingEditor_.Edit();
    easingTestObject_->Update();

    // 通常のパーティクル更新
    KetaEngine::ParticleManager::GetInstance()->Update();

    // GPUパーティクル更新
    KetaEngine::GPUParticleManager::GetInstance()->Update();
    ViewProjectionUpdate();

    if (input_->TriggerKey(KeyboardKey::Enter)) {
        KetaEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void EditorScene::SkyBoxDraw() {
}

void EditorScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
    ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
    ImGui::End();

    BaseScene::Debug();

    easingTestObject_->Debug();

  

#endif
}

void EditorScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void EditorScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}