/// scene
#include "EditorScene.h"
#include "GameScene.h"
#include "Manager/SceneManager.h"

#include "base/TextureManager.h"
// class
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
}

void EditorScene::Update() {
    
     easingEditor_.Edit();
    easingTestObject_->Update();

    ParticleManager::GetInstance()->Update();

    Debug();
    ViewProjectionUpdate();

    if (input_->TrrigerKey(DIK_RETURN)) {

        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

/// ===================================================
/// モデル描画
/// ===================================================
void EditorScene::ModelDraw() {
    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
    Model::PreDraw(commandList);

     easingTestObject_->Draw(viewProjection_);

    ParticleManager::GetInstance()->Draw(viewProjection_);
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void EditorScene::SkyBoxDraw() {
}

/// ===================================================
/// スプライト描画
/// ===================================================
void EditorScene::SpriteDraw() {
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

void EditorScene::ViewProssess() {
    viewProjection_.UpdateMatrix();
}
