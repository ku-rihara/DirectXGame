/// scene
#include "TitleScene.h"
#include "GameScene.h"
#include "Scene/Manager/SceneManager.h"

// class
#include "Editor/ParticleEditor/ParticleManager.h"

// math
#include <imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
}

void TitleScene::Init() {

    BaseScene::Init();

    /// パーティクルデータの読み込みと、モデルの読み込み
    EnemydamageEffect_[0].reset(KetaEngine::ParticleEmitter::CreateParticlePrimitive("comboFireNozzleLeft", PrimitiveType::Plane, 500));
    EnemydamageEffect_[1].reset(KetaEngine::ParticleEmitter::CreateParticlePrimitive("comboFireCenter", PrimitiveType::Plane, 500));
    EnemydamageEffect_[2].reset(KetaEngine::ParticleEmitter::CreateParticlePrimitive("comboFireNozzleRigth", PrimitiveType::Plane, 500));

    afterGlowEffect_[0].reset(KetaEngine::ParticleEmitter::CreateParticle("afterGlowEffect", "Suzanne.obj", 500));

    KetaEngine::ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);

    // GPUパーティクルの初期化
    test.Init();
    test.InitEffect("Player", "test1");
}

void TitleScene::Update() {

    BaseScene::Update();

 
    if (input_->GetInstance()->PushKey(KeyboardKey::P)) {
        test.Play("Player", "test1");
    }

    // 常に更新
    test.Update();

    /// 既存のパーティクル処理
    for (int i = 0; i < EnemydamageEffect_.size(); i++) {
        EnemydamageEffect_[i]->Update();
        EnemydamageEffect_[i]->EditorUpdate();
        EnemydamageEffect_[i]->Emit();

        if (KetaEngine::Input::GetInstance()->TriggerKey(KeyboardKey::O)) {
            EnemydamageEffect_[i]->StartRailEmit();
        }
    }

    for (int i = 0; i < afterGlowEffect_.size(); i++) {
        afterGlowEffect_[i]->Update();
        afterGlowEffect_[i]->EditorUpdate();
        afterGlowEffect_[i]->Emit();
    }

    KetaEngine::ParticleManager::GetInstance()->Update();

    ViewProjectionUpdate();

    if (input_->TriggerKey(KeyboardKey::Enter)) {
        KetaEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void TitleScene::SkyBoxDraw() {
}

void TitleScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
    ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
    ImGui::End();

    ImGui::Begin("GPU Particle Debug");
    auto* particleData = test.GetParticleData();
    if (particleData) {
        bool isPlaying = particleData->IsPlaying();
        ImGui::Text("Is Playing: %s", isPlaying ? "YES" : "NO");
        ImGui::Text("P Key Status: %s",
            input_->GetInstance()->PushKey(KeyboardKey::P) ? "PRESSED" : "RELEASED");

        if (ImGui::Button("Reset")) {
            test.Reset();
        }
    }
    ImGui::End();
#endif
}

// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void TitleScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}