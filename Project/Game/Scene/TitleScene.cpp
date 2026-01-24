/// scene
#include "TitleScene.h"
#include "GameScene.h"
#include "Scene/Manager/SceneManager.h"

// math
#include <imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
}

void TitleScene::Init() {

    BaseScene::Init();

   
    // GPUパーティクルの初期化
    test.Init();
    test.InitEffect("Player", "test1");

    particleTestPlayer_.Init();
}

void TitleScene::Update() {

    BaseScene::Update();

  
    if (input_->GetInstance()->TriggerKey(KeyboardKey::P)) {
      /*  test.Play("Player", "test1");*/
        particleTestPlayer_.Play("Enemy", "NormalDamageEffect");
    }
    particleTestPlayer_.Update();

    // 常に更新
    test.Update();

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