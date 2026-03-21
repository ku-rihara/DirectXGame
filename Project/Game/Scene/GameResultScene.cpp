#include "GameResultScene.h"
#include "GameClearInfo/GameClearInfo.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "Scene/Manager/SceneManager.h"
#include "Base/TextureManager.h"

void GameResultScene::Init() {
    BaseScene::Init();

    bgSprite_.reset(KetaEngine::Sprite::Create("white1x1.dds"));

    comboCountUI_ = std::make_unique<KillCountUIController>();
    comboCountUI_->Init();
}

void GameResultScene::Update() {
    BaseScene::Update();

    comboCountUI_->Update(GameClearInfo::GetInstance()->GetMaxComboCount());

    if (!isStartFadeOut_) {
        CheckEndInput();
        return;
    }

    alpha_ += KetaEngine::Frame::DeltaTime();
    if (alpha_ >= 1.0f) {
        GameClearInfo::GetInstance()->Reset();
        KetaEngine::SceneManager::GetInstance()->ChangeScene("TITLE");
    }
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
#endif
}

void GameResultScene::ViewProjectionUpdate() {
    BaseScene::ViewProjectionUpdate();
}
