#include "GameResultScene.h"
#include "ResultObj/GameResultInfo.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "Scene/Manager/SceneManager.h"
#include "Base/TextureManager.h"
#include <imgui.h>

void GameResultScene::Init() {
    BaseScene::Init();

    bgSprite_.reset(KetaEngine::Sprite::Create("default.dds"));

    skyBox_ = std::make_unique<SkyBox>();
    skyBox_->Init();

    resultStage_ = std::make_unique<ResultStage>();
    resultStage_->Init("ResultScene.json");

    resultRunner_ = std::make_unique<ResultRunner>();
    resultRunner_->Init();

    resultUI_ = std::make_unique<GameResultUI>();
    resultUI_->Init();
}

void GameResultScene::Update() {
    BaseScene::Update();

    skyBox_->Update();
    resultStage_->Update();
    resultRunner_->Update();
    resultUI_->Update();

    ViewProjectionUpdate();

  
    if (!isStartFadeOut_) {
        CheckEndInput();
        return;
    }

  /*  alpha_ += KetaEngine::Frame::DeltaTime();
    if (alpha_ >= 1.0f) {
        GameResultInfo::GetInstance()->Reset();
        KetaEngine::SceneManager::GetInstance()->ChangeScene("TITLE");
    }*/
}

void GameResultScene::CheckEndInput() {
    if (KetaEngine::Input::GetInstance()->TriggerKey(KeyboardKey::Space) ||
        KetaEngine::Input::IsTriggerPad(0, GamepadButton::A)) {
        isStartFadeOut_ = true;
    }
}

void GameResultScene::Debug() {
#ifdef _DEBUG
    BaseScene::Debug();
    ImGui::Begin("Param");
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
