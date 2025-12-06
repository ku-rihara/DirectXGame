/// scene
#include "EditorScene.h"
#include "GameScene.h"
#include "Manager/SceneManager.h"

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

    // GPUParticleエミッター作成例
    testGPUParticle_.reset(
        GPUParticleEmitter::CreateParticlePrimitive(
            "TestGPUParticle",
            PrimitiveType::Plane,
            10000));

    // エフェクトのプリセット例
    fireEffect_.reset(
        GPUParticleEmitter::CreateParticlePrimitive(
            "FireEffect",
            PrimitiveType::Plane,
            5000));

    explosionEffect_.reset(
        GPUParticleEmitter::CreateParticlePrimitive(
            "ExplosionEffect",
            PrimitiveType::Plane,
            3000));
}

void EditorScene::Update() {

    BaseScene::Update();

    easingEditor_.Edit();
    easingTestObject_->Update();

    // 通常のパーティクル更新
    ParticleManager::GetInstance()->Update();

    // GPUパーティクル更新
    GPUParticleManager::GetInstance()->Update();

    // エミッター更新
    if (testGPUParticle_) {
        testGPUParticle_->Update();
        testGPUParticle_->Emit();
    }

    if (fireEffect_) {
        fireEffect_->Update();
        fireEffect_->Emit();
    }

    if (explosionEffect_) {
        explosionEffect_->Update();
        explosionEffect_->Emit();
    }

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

    // GPUパーティクルエディタ
    ImGui::Begin("GPU Particle Editor");

    if (ImGui::CollapsingHeader("Test GPU Particle")) {
        if (testGPUParticle_) {
            testGPUParticle_->EditorUpdate();
        }
    }

    if (ImGui::CollapsingHeader("Fire Effect")) {
        if (fireEffect_) {
            fireEffect_->EditorUpdate();
        }
    }

    if (ImGui::CollapsingHeader("Explosion Effect")) {
        if (explosionEffect_) {
            explosionEffect_->EditorUpdate();
        }
    }

    ImGui::End();
#endif
}

void EditorScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void EditorScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}