#include "GameScene.h"
// editor
#include "Editor/AudioDSPEditor/AudioDSPEditorFactory.h"
// Frame
#include "Frame/Frame.h"
// ImGui
#include <imgui.h>

void GameScene::Init() {
    BaseScene::Init();

    // SkyBox生成
    skyBox_ = std::make_unique<KetaEngine::SkyBox>();
    skyBox_->Init();

    // 原点の地面生成
    ground_ = std::make_unique<KetaEngine::Ground>();
    ground_->Init();

    // サウンド編集エディター(標準構成)
    audioDSPEditor_ = KetaEngine::AudioDSPEditorFactory().Create(*audio_);

    // 地面を見下ろす位置にカメラを置く
    viewProjection_.translation_ = {0.0f, 18.0f, -55.0f};
    viewProjection_.rotation_    = {0.25f, 0.0f, 0.0f};

    KetaEngine::Frame::ResetDeltaTime();
}

void GameScene::Update() {
    BaseScene::Update();

    // カメラ更新(通常/Editor/デバッグの切り替えを含む)
    ViewProjectionUpdate();

    skyBox_->Update();
    ground_->Update();
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void GameScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void GameScene::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    BaseScene::Debug();

    // サウンド編集
    ImGui::Begin("サウンド編集");
    audioDSPEditor_->EditorUpdate();
    ImGui::End();

    ImGui::Begin("ParameterEditor");
    ground_->AdjustParam();
    skyBox_->Debug();
    ImGui::End();
#endif
}

void GameScene::ViewProcess() {
    // 通常カメラはImGuiで編集した値をそのまま使う
    viewProjection_.UpdateMatrix();
}
