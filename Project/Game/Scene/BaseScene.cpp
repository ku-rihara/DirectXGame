#include "BaseScene.h"
#include "Base/WinApp.h"
#include "Frame/Frame.h"
#include "Light/Light.h"
#include "PostEffect/PostEffectRenderer.h"

// editor
#include "Editor/CameraEditor/CameraEditor.h"

// Particle
#include "Particle/CPUParticle/ParticleManager.h"
#include "Particle/GPUParticle/GPUParticleManager.h"

#include <imgui.h>

BaseScene::~BaseScene() {
    if (KetaEngine::PostEffectRenderer::GetInstance()) {
        KetaEngine::PostEffectRenderer::GetInstance()->SetViewProjection(nullptr);
    }
    if (KetaEngine::ParticleManager::GetInstance()) {
        KetaEngine::ParticleManager::GetInstance()->SetViewProjection(nullptr);
    }
    if (KetaEngine::GPUParticleManager::GetInstance()) {
        KetaEngine::GPUParticleManager::GetInstance()->SetViewProjection(nullptr);
    }
}

void BaseScene::Init() {

    // メンバ変数の初期化
    input_          = KetaEngine::Input::GetInstance();
    audio_          = KetaEngine::Audio::GetInstance();
    textureManager_ = KetaEngine::TextureManager::GetInstance();

    // 生成
    debugCamera_       = std::make_unique<KetaEngine::DebugCamera>();
    effectEditorSuite_ = std::make_unique<KetaEngine::EffectEditorSuite>();

    // 初期化
    debugCamera_->Init();
    effectEditorSuite_->Init();
    viewProjection_.Init();

    // ビュープロジェクションのセットアップ
    SetupViewProjection();

    // カメラモードのテーブル登録
    RegisterCameraUpdaters();
    RegisterCameraTransitions();
}

void BaseScene::SetupViewProjection() {
  
    // 各システムにviewProjectionを渡す
    KetaEngine::PostEffectRenderer::GetInstance()->SetViewProjection(&viewProjection_);
    KetaEngine::ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
    KetaEngine::GPUParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
    effectEditorSuite_->SetViewProjection(&viewProjection_);
}

void BaseScene::RegisterCameraUpdaters() {

    // 通常カメラの更新処理の登録
    cameraUpdaters_[CameraMode::NORMAL] = [this]() {
        ViewProcess();
    };
    // Editorカメラの更新処理の登録
    cameraUpdaters_[CameraMode::EDITOR] = [this]() {
        viewProjection_.UpdateMatrix();
    };

    // デバッグカメラの更新処理の登録
    cameraUpdaters_[CameraMode::DEBUG] = [this]() {
        // デバッグカメラの更新
        debugCamera_->Update();
        // カメラ行列をDebugCameraのViewProjectionから取得する
        viewProjection_.matView_       = debugCamera_->GetViewProjection().matView_;
        viewProjection_.matProjection_ = debugCamera_->GetViewProjection().matProjection_;
        viewProjection_.cameraMatrix_  = debugCamera_->GetViewProjection().cameraMatrix_;
    };
}

void BaseScene::RegisterCameraTransitions() {

    // 通常カメラからの遷移条件の登録
    cameraTransitions_[CameraMode::NORMAL] = [this]() {
        CameraMode next = CameraMode::NORMAL;
        // スペース入力でDebugカメラへの移行
        if (input_->TriggerKey(KeyboardKey::Space)) {
            next = CameraMode::DEBUG;
        }
        // カメラEditorモードへの移行
        if (effectEditorSuite_->GetCameraEditor()->GetIsEditing()) {
            next = CameraMode::EDITOR;
        }
        return next;
    };

    // Editorカメラからの遷移条件の登録
    cameraTransitions_[CameraMode::EDITOR] = [this]() {
        CameraMode next = CameraMode::EDITOR;
        // 通常カメラへの移行
        if (!effectEditorSuite_->GetCameraEditor()->GetIsEditing()) {
            next = CameraMode::NORMAL;
        }
        // スペース入力でDebugカメラへ移行
        if (input_->TriggerKey(KeyboardKey::Space)) {
            next = CameraMode::DEBUG;
        }
        return next;
    };

    // デバッグカメラからの遷移条件の登録
    cameraTransitions_[CameraMode::DEBUG] = [this]() {
        CameraMode next = CameraMode::DEBUG;
        // スペース入力で通常カメラへ移行
        if (input_->TriggerKey(KeyboardKey::Space)) {
            next = CameraMode::NORMAL;
        }
        return next;
    };
}

void BaseScene::Update() {
    // エディター機能更新
    UpdateEditors();
}

void BaseScene::UpdateEditors() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    // デバッグカメラとEditor更新
    debugCamera_->Update();
    effectEditorSuite_->Update();
#endif
}

void BaseScene::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    // ViewProjectionのTransform編集
    ImGui::Begin("Camera");
    ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
    ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
    ImGui::End();

    // エディター編集
    ImGui::Begin("Effect Editor Suite");
    effectEditorSuite_->EditorUpdate();
    ImGui::End();

    // ImGuiの描画
    KetaEngine::PostEffectRenderer::GetInstance()->DrawImGui();
#endif
}

void BaseScene::ViewProjectionUpdate() {
#if defined(_DEBUG) || defined(DEVELOPMENT)

    // 現在のモードに応じて次のカメラモードを決定する
    if (auto it = cameraTransitions_.find(cameraMode_); it != cameraTransitions_.end()) {
        cameraMode_ = it->second();
    }
  
#endif
    // 現在のカメラモードに対応する更新処理をテーブルから実行する
    if (auto it = cameraUpdaters_.find(cameraMode_); it != cameraUpdaters_.end()) {
        it->second();
    }
   
    // ライトにカメラ位置を渡す
    KetaEngine::Light::GetInstance()->SetWorldCameraPos(viewProjection_.GetWorldPos());
}
