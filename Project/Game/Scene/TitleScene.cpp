/// scene
#include "TitleScene.h"
#include "Scene/Manager/SceneManager.h"
// 2D
#include "2d/SpriteRegistry.h"
// Light
#include "Lighrt/Light.h"


// math
#include <imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Init() {
    //// グローバル変数の読み込み
    KetaEngine::GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();

    ///=======================================================================================
    /// 生成
    ///=======================================================================================
    field_             = std::make_unique<Field>();
    player_            = std::make_unique<Player>();
    skyBox_            = std::make_unique<SkyBox>();
    ObjectFromBlender_ = std::make_unique<KetaEngine::ObjectFromBlender>();

    // 演出・遷移の統括クラス
    sceneController_ = std::make_unique<TitleSceneController>();

    ///=======================================================================================
    /// 初期化
    ///=======================================================================================
    field_->Init();
    skyBox_->Init();
    player_->Init();
    ObjectFromBlender_->LoadJsonFile("gameScene.json");

    // プレイヤー設定
    player_->SetTitleBehavior();
    player_->SetWorldPositionY(30.0f);

    // 演出・遷移クラス初期化
    sceneController_->Init(player_.get());

    // カメラ設定
    viewProjection_.translation_ = {7.8f, 3.6f, 8.3f};
    viewProjection_.rotation_.y  = 3.8f;

}

void TitleScene::Update() {
    BaseScene::Update();

    // 演出・遷移の統括更新
    sceneController_->Update();

    // 基本オブジェクト更新
    field_->Update();
    skyBox_->Update();

    // カメラ更新
    ViewProjectionUpdate();

    // シーン切り替え判定
    if (sceneController_->IsTransitionComplete()) {
        KetaEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void TitleScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void TitleScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Param");
    KetaEngine::Light::GetInstance()->DebugImGui();
    KetaEngine::SpriteRegistry::GetInstance()->DebugImGui();
    ImGui::End();
#endif
}

// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
    KetaEngine::Light::GetInstance()->SetWorldCameraPos(player_->GetWorldPosition());
}

void TitleScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}