#include "GameResultScene.h"
#include "ResultObj/GameResultInfo.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "Scene/Manager/SceneManager.h"
#include "Base/TextureManager.h"

void GameResultScene::Init() {
    BaseScene::Init();

    bgSprite_.reset(KetaEngine::Sprite::Create("default.dds"));

    comboCountUI_ = std::make_unique<KillCountUIController>();
    comboCountUI_->SetGroupName("ResultComboCountUI");
    comboCountUI_->Init();

    killCountUI_ = std::make_unique<KillCountUIController>();
    killCountUI_->SetGroupName("ResultKillCountUI");
    killCountUI_->Init();

    levelUI_.Init();
}

void GameResultScene::Update() {
    BaseScene::Update();

    auto* info = GameResultInfo::GetInstance();
    comboCountUI_->Update(info->GetMaxComboCount());
    killCountUI_->Update(info->GetTotalKillCount());
    levelUI_.Update(info->GetReachedLevel());

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
    BaseScene::Debug();

#ifdef _DEBUG
    comboCountUI_->AdjustParam();
    killCountUI_->AdjustParam();
    levelUI_.AdjustParam();
#endif
}

void GameResultScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}
