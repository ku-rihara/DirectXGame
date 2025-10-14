/// scene
#include "TitleScene.h"
#include "3d/Object3DRegistry.h"
#include"2d/SpriteRegistry.h"
#include "Lighrt/Light.h"
#include "Manager/SceneManager.h"
#include "Player/TitleBehavior/TitleRightPunch.h"
#include "utility/ParticleEditor/ParticleManager.h"

#include "base/TextureManager.h"
#include "Frame/Frame.h"

// math
#include <imgui.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {
}

void TitleScene::Init() {
    //// グローバル変数の読み込み
    GlobalParameter::GetInstance()->LoadFiles();
    BaseScene::Init();
    ///=======================================================================================
    /// 生成
    ///=======================================================================================

    field_            = std::make_unique<Field>();
    player_           = std::make_unique<Player>();
    skydome_          = std::make_unique<Skydome>();
    titleRogo_        = std::make_unique<TitleRogo>();
    skyBox_           = std::make_unique<SkyBox>();
    putObjForBlender_ = std::make_unique<PutObjForBlender>();

    ///=======================================================================================
    /// 初期化
    ///=======================================================================================
    field_->Init();
    skyBox_->Init();
    player_->Init();
    titleRogo_->Init();
    putObjForBlender_->LoadJsonFile("gameScene.json");

    alpha_ = 0.0f;

    viewProjection_.translation_ = {7.8f, 3.6f, 8.3f};
    viewProjection_.rotation_.y  = 3.8f;

    screenSprite_.reset(Sprite::Create("screenChange.png"));

    player_->SetTitleBehavior();
    player_->SetWorldPositionY(30.0f);

    ParticleManager::GetInstance()->SetViewProjection(&viewProjection_);
}

void TitleScene::Update() {

    screenSprite_->SetAlpha(alpha_);

    player_->TitleUpdate();
    field_->Update();
    skyBox_->Update();

    if (dynamic_cast<TitleRightPunch*>(player_->GetTitleBehavior())) {
        if (!isTitleRogoUpdate) {
            isTitleRogoUpdate = true;
        }
    }
    if (isTitleRogoUpdate) {
        titleRogo_->Update();
    }

    Object3DRegistry::GetInstance()->UpdateAll();
    ParticleManager::GetInstance()->Update();

    // 　ゲーム遷移
    if (Input::GetInstance()->PushKey(DIK_SPACE)) {
        isFinished_ = true;
    } else {
        ChangeForJoyState(); // コントローラジャンプ
    }

    Debug();
    ViewProjectionUpdate();

    if (isFinished_) {
        alpha_ += Frame::DeltaTime();
        if (alpha_ >= 1.2f) {
            SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
        }
    }
}

void TitleScene::ChangeForJoyState() {

    if (!((Input::IsTriggerPad(0, XINPUT_GAMEPAD_A)))) {
        return;
    }

    isFinished_ = true;
}

/// ===================================================
/// SkyBox描画
/// ===================================================
void TitleScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void TitleScene::Debug() {
#ifdef _DEBUG
    ImGui::Begin("Camera");
    ImGui::DragFloat3("pos", &viewProjection_.translation_.x, 0.1f);
    ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.1f);
    ImGui::End();

    ImGui::Begin("Param");
    Light::GetInstance()->DebugImGui();
    SpriteRegistry::GetInstance()->DebugImGui();
    ImGui::End();
#endif
}

// ビュープロジェクション更新
void TitleScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
    Light::GetInstance()->SetWorldCameraPos(player_->GetWorldPosition());
}

void TitleScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}