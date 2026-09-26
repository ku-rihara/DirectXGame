#include "EngineScene.h"
// Frame
#include "Frame/Frame.h"
// ImGui
#include <imgui.h>

void EngineScene::Init() {
    BaseScene::Init();

    // SkyBox生成
    skyBox_ = std::make_unique<KetaEngine::SkyBox>();
    skyBox_->Init();

    // 原点の地面生成
    ground_ = std::make_unique<KetaEngine::Ground>();
    ground_->Init();

    // 地面を見下ろす位置にカメラを置く
    viewProjection_.translation_ = {0.0f, 18.0f, -55.0f};
    viewProjection_.rotation_    = {0.25f, 0.0f, 0.0f};

    KetaEngine::Frame::ResetDeltaTime();
}

void EngineScene::Update() {
    BaseScene::Update();

    // カメラ更新(通常/Editor/デバッグの切り替えを含む)
    ViewProjectionUpdate();

    skyBox_->Update();
    ground_->Update();
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void EngineScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void EngineScene::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    BaseScene::Debug();

    ImGui::Begin("ParameterEditor");
    ground_->AdjustParam();
    skyBox_->Debug();
    ImGui::End();
#endif
}

void EngineScene::ViewProcess() {
    // 通常カメラはImGuiで編集した値をそのまま使う
    viewProjection_.UpdateMatrix();
}
