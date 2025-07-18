/// scene
#include "TitleScene.h"
#include "GameScene.h"
#include "Manager/SceneManager.h"

#include "base/TextureManager.h"
// class
#include "utility/ParticleEditor/ParticleManager.h"

// math
#include "Frame/Frame.h"
#include <imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
}

void TitleScene::Init() {

    BaseScene::Init();
    plane_ = std::make_unique<Plane>();
    plane_->Init();

    /// パーティクルデータの読み込みと、モデルの読み込み
    EnemydamageEffect_[0].reset(ParticleEmitter::CreateParticlePrimitive("comboFireNozzleLeft", PrimitiveType::Plane, 500));
    EnemydamageEffect_[1].reset(ParticleEmitter::CreateParticlePrimitive("comboFireCenter", PrimitiveType::Plane, 500));
    EnemydamageEffect_[2].reset(ParticleEmitter::CreateParticlePrimitive("comboFireNozzleRigth", PrimitiveType::Plane, 500));
    ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
}

void TitleScene::Update() {
    plane_->Update();
    ///
    for (int i = 0; i < EnemydamageEffect_.size(); i++) {
        EnemydamageEffect_[i]->Update();
        EnemydamageEffect_[i]->EditorUpdate();
        EnemydamageEffect_[i]->Emit();

        if (Input::GetInstance()->TrrigerKey(DIK_O)) {
            EnemydamageEffect_[i]->StartRailEmit();
        }
    }
    /* EnemydamageEffect_[0]->Update();
     EnemydamageEffect_[0]->EditorUpdate();
     EnemydamageEffect_[0]->Emit();*/

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
void TitleScene::ModelDraw() {
    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
    Model::PreDraw(commandList);
    plane_->Draw(viewProjection_);
    EnemydamageEffect_[0]->DebugDraw(viewProjection_);
    ParticleManager::GetInstance()->Draw(viewProjection_);
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void TitleScene::SkyBoxDraw() {
}

/// ===================================================
/// スプライト描画
/// ===================================================
void TitleScene::SpriteDraw() {
}

void TitleScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
    ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
    ImGui::End();
#endif
}

// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void TitleScene::ViewProssess() {
    viewProjection_.UpdateMatrix();
}
