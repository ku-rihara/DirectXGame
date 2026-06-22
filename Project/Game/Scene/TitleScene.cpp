/// scene
#include "TitleScene.h"
#include "Scene/Manager/SceneManager.h"
// 2D
#include "2d/SpriteRegistry.h"
// Light
#include "Light/Light.h"


// math
#include <imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Init() {

    // グローバル変数の読み込み
    KetaEngine::GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();

    ///=======================================================================================
    /// 生成
    ///=======================================================================================
    field_             = std::make_unique<Field>();
    player_            = std::make_unique<TitlePlayer>();
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
    player_->SetWorldPositionY(0.0f); 

    // 演出・遷移クラス初期化
    sceneController_->Init(player_.get());

    // GlobalParameter登録
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void TitleScene::Update() {
    BaseScene::Update();

    // 演出・遷移の統括更新
    sceneController_->Update();

    // オブジェクト更新
    field_->Update();
    skyBox_->Update();

    // カメラ更新
    ViewProjectionUpdate();

    // シーン切り替え判定
    if (sceneController_->IsTransitionComplete()) {
        KetaEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }
}

void TitleScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void TitleScene::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGui::Begin("Param");
    // 演出Editor更新
    effectEditorSuite_->EditorUpdate();

    // ライトとスプライトのデバッグ
    KetaEngine::Light::GetInstance()->DebugImGui();
    KetaEngine::SpriteRegistry::GetInstance()->DebugImGui();

    // タイトルシーンのカメラ設定
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::DragFloat3("cameraTranslation", &viewProjection_.translation_.x, 0.01f);
        ImGui::DragFloat3("cameraRotation", &viewProjection_.rotation_.x, 0.01f);
       
        // セーブ、ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
    }

    ImGui::End();
#endif
}

void TitleScene::RegisterParams() {
    globalParameter_->Regist(groupName_, "cameraTranslation", &viewProjection_.translation_);
    globalParameter_->Regist(groupName_, "cameraRotation", &viewProjection_.rotation_);
}

// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
    KetaEngine::Light::GetInstance()->SetWorldCameraPos(player_->GetWorldPosition());
}

void TitleScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}