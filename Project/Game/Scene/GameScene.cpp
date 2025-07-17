#include "GameScene.h"
#include "base/TextureManager.h"
// class
#include "utility/ParticleEditor/ParticleManager.h"

// math
#include "Frame/Frame.h"
#include "Lighrt/Light.h"
#include "Scene/Manager/SceneManager.h"

#include <imgui.h>

GameScene::GameScene() {}

GameScene::~GameScene() {
}

void GameScene::Init() {

    BaseScene::Init();

    ground_          = std::make_unique<Ground>();
    monsterBall_     = std::make_unique<MonsterBall>();
    plane_           = std::make_unique<Plane>();
    skuBox_          = std::make_unique<SkyBox>();
    putObjForBlender = std::make_unique<PutObjForBlender>();

    monsterBall_->Init();
    ground_->Init();
    plane_->Init();
    skuBox_->Init();
    putObjForBlender->LoadJsonFile("game.json");
    putObjForBlender->PutObject();
    putObjForBlender->EasingAllReset();

    isDebugCameraActive_ = true;
    ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
}

void GameScene::Update() {

    /// debugCamera
    debugCamera_->Update();
    Debug();

   
    // 各クラス更新
    ground_->Update();
    monsterBall_->Update();
    plane_->Update();
    skuBox_->Update();

    putObjForBlender->EasingUpdateSelectGroup(Frame::DeltaTime(), 0);

    // viewProjection 更新
    ViewProjectionUpdate();

    if (input_->TrrigerKey(DIK_RETURN)) {
        SceneManager::GetInstance()->ChangeScene("TITLE");
    }

    ParticleManager::GetInstance()->Update();
}

/// ===================================================
/// モデル描画
/// ===================================================
void GameScene::ModelDraw() {
    /// commandList取得
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
    Model::PreDraw(commandList);

    putObjForBlender->DrawAll(viewProjection_);
    ground_->Draw(viewProjection_);
    plane_->Draw(viewProjection_);
    monsterBall_->Draw(viewProjection_);

    ParticleManager::GetInstance()->Draw(viewProjection_);
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void GameScene::SkyBoxDraw() {
    skuBox_->Draw(viewProjection_);
}

/// ======================================================
/// スプライト描画
/// ======================================================
void GameScene::SpriteDraw() {
}

void GameScene::Debug() {
#ifdef _DEBUG

    Light::GetInstance()->DebugImGui();
    ImGui::Begin("Object");
    ground_->Debug();
    monsterBall_->Debug();
    plane_->Debug();
    skuBox_->Debug();

    ImGui::End();

#endif
}

// ビュープロジェクション更新
void GameScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameScene::ViewProssess() {
    viewProjection_.UpdateMatrix();
}
