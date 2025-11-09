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
    debugCamera_            = std::make_unique<DebugCamera>(WinApp::kWindowWidth, WinApp::kWindowHeight);
    cameraEditor_           = std::make_unique<CameraEditor>();
    shakeEditor_            = std::make_unique<ShakeEditor>();
    railEditor_             = std::make_unique<RailEditor>();
    objEaseAnimationEditor_ = std::make_unique<ObjEaseAnimationEditor>();

    // 初期化
    debugCamera_->Init();
    cameraEditor_->Init(&viewProjection_);
    shakeEditor_->Init();
    railEditor_->Init();
    objEaseAnimationEditor_->Init();
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
    // シーンごとの切り替え処理------------------------------
    bool isTriggerSpace = input_->TriggerKey(KeyboardKey::Space);
    switch (cameraMode_) {
        ///------------------------------------------------------
        /// Normal Mode
        ///------------------------------------------------------
    case BaseScene::CameraMode::NORMAL:
        // デバッグモードへ
        if (isTriggerSpace) {
            cameraMode_ = CameraMode::DEBUG;
        }
        // エディターモードへ
        if (cameraEditor_->GetIsEditing()) {
            cameraMode_ = CameraMode::EDITOR;
        }
        break;
        ///------------------------------------------------------
        /// Editor Mode
        ///------------------------------------------------------
    case BaseScene::CameraMode::EDITOR:
        // ノーマルモードへ
        if (!cameraEditor_->GetIsEditing()) {
            cameraMode_ = CameraMode::NORMAL;
        }
        // デバッグモードへ
        if (isTriggerSpace) {
            cameraMode_ = CameraMode::DEBUG;
        }
        break;
        ///------------------------------------------------------
        /// Debug Mode
        ///------------------------------------------------------
    case BaseScene::CameraMode::DEBUG:
        if (isTriggerSpace) {
            cameraMode_ = CameraMode::NORMAL;
        }
        break;
    default:
        break;
    }

#endif

    switch (cameraMode_) {
    case BaseScene::CameraMode::NORMAL:
        ViewProcess();
        break;
    case BaseScene::CameraMode::EDITOR:

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
