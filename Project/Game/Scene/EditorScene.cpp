/// scene
#include "EditorScene.h"
#include "GameScene.h"
#include "Manager/SceneManager.h"

#include "base/TextureManager.h"
// class
#include "3d/Object3DRegistry.h"
#include "Pipeline/PipelineManager.h"
#include "utility/ParticleEditor/ParticleManager.h"

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

    ParticleManager::GetInstance()->Update();

    ViewProjectionUpdate();

    if (input_->TriggerKey(KeyboardKey::Enter)) {

        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
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

    easingTestObject_->Debug();
#endif
}

// ビュープロジェクション更新
void EditorScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void EditorScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}
