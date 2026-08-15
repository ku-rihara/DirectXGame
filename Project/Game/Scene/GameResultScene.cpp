#include "GameResultScene.h"
#include "audio/Audio.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "ResultObj/GameResultInfo.h"
#include "Scene/Manager/SceneManager.h"
#include <imgui.h>

void GameResultScene::Init() {
    BaseScene::Init();

    // 背景スプライトの初期化
    bgSprite_.reset(KetaEngine::Sprite::Create("screenChange.dds"));
    bgSprite_->SetIsDraw(true);
    bgSprite_->SetAlpha(0.f);
    alpha_ = 0.f;

    // SkyBoxの初期化
    skyBox_ = std::make_unique<SkyBox>();
    skyBox_->Init();

    // ステージオブジェクトの初期化
    resultStage_ = std::make_unique<ResultStage>();
    resultStage_->Init("ResultScene.json");
    resultStage_->SetViewProjection(&viewProjection_);

    // 敗走する敵の初期化
    resultRunner_ = std::make_unique<ResultRunner>();
    resultRunner_->Init();

    // UIの初期化
    resultUI_ = std::make_unique<GameResultUI>();
    resultUI_->Init();
}

void GameResultScene::Update() {
    BaseScene::Update();
    KetaEngine::Frame::SetTimeScale(1.0f);

    // 各オブジェクトの更新
    skyBox_->Update();
    resultStage_->Update();
    resultRunner_->Update();
    resultUI_->Update();

    ViewProjectionUpdate();

    if (!isStartFadeOut_) {
        CheckEndInput();
        return;
    }

    alpha_ += KetaEngine::Frame::DeltaTime();
    bgSprite_->SetAlpha(alpha_);
    if (alpha_ >= 1.0f) {
        GameResultInfo::GetInstance()->Reset();
        KetaEngine::SceneManager::GetInstance()->ChangeScene("TITLE");
    }
}

void GameResultScene::CheckEndInput() {
    if (KetaEngine::Input::GetInstance()->TriggerKey(KeyboardKey::Space) || KetaEngine::Input::IsTriggerPad(0, GamepadButton::A)) {
        isStartFadeOut_ = true;
        KetaEngine::Audio::GetInstance()->Play("SelectSE.mp3", 1.0f);
    }
}

void GameResultScene::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    BaseScene::Debug();
    ImGui::Begin("Param");
    resultStage_->AdjustParam();
    resultRunner_->AdjustParam();
    resultUI_->AdjustParam();
    ImGui::End();
#endif
}

void GameResultScene::SkyBoxDraw() {
    skyBox_->Draw(viewProjection_);
}

void GameResultScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}

void GameResultScene::ViewProcess() {
    viewProjection_.UpdateMatrix();
}
