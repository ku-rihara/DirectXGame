#include "BaseScene.h"
#include "base/WinApp.h"
#include "Lighrt/Light.h"
#include "PostEffect/PostEffectRenderer.h"

// Particle
#include "GPUParticle/GPUParticleManager.h"
#include "utility/ParticleEditor/ParticleManager.h"

#include <imgui.h>

void BaseScene::Init() {

    // メンバ変数の初期化
    input_          = Input::GetInstance();
    audio_          = Audio::GetInstance();
    textureManager_ = TextureManager::GetInstance();

    // 生成
    debugCamera_  = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);
    cameraEditor_ = std::make_unique<CameraEditor>();
    shakeEditor_  = std::make_unique<ShakeEditor>();

    // 初期化
    debugCamera_->Init();
    cameraEditor_->Init(&viewProjection_);
    shakeEditor_->Init();
    viewProjection_.Init();

    // ビュープロジェクション
    viewProjection_.translation_ = {0, -6.2f, -109.0f};

    // viewProjectionのセット
    PostEffectRenderer::GetInstance()->SetViewProjection(&viewProjection_);
    ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
    GPUParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
}

void BaseScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
    ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
    ImGui::End();
#endif
}

// ビュープロジェクション更新
void BaseScene::ViewProjectionUpdate() {

#ifdef _DEBUG
    // デバッグカメラモード切り替え------------------------------
    if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
        switch (cameraMode_) {
        case BaseScene::CameraMode::NORMAL:
            cameraMode_ = CameraMode::DEBUG;
            break;
        case BaseScene::CameraMode::EDITOR:
            cameraMode_ = CameraMode::DEBUG;
            break;
        case BaseScene::CameraMode::DEBUG:
            cameraMode_ = CameraMode::NORMAL;
            break;
        default:
            break;
        }
    }

    // デバッグカメラモード切り替え------------------------------
#endif

    switch (cameraMode_) {
    case BaseScene::CameraMode::NORMAL:
        ViewProcess();
        break;
    case BaseScene::CameraMode::EDITOR:
        cameraMode_ = CameraMode::DEBUG;
        break;
    case BaseScene::CameraMode::DEBUG:
        // デバッグカメラの更新
        debugCamera_->Update();
        // カメラ行列の計算をDebugCameraのViewProjectionから行う
        viewProjection_.matView_       = debugCamera_->GetViewProjection().matView_;
        viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;
        viewProjection_.cameraMatrix_  = debugCamera_->GetViewProjection().cameraMatrix_;
        break;
    default:
        break;
    }

    Light::GetInstance()->SetWorldCameraPos(viewProjection_.GetWorldPos());
}
