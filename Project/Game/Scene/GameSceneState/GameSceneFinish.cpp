#include "GameSceneFinish.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "Scene/GameScene.h"
#include "Scene/Manager/SceneManager.h"

GameSceneFinish::GameSceneFinish(GameScene* gameScene)
    : BaseGameSceneState("GameSceneFinish", gameScene) {
    Init();
}

void GameSceneFinish::Init() {
    alpha_ = 0.0f;
    isStartFadeOut_ = false;

    // スプライト初期化
    if (pOwner_->GetGameObj().player_->GetIsDeathRenditionFinish()) {
        clearSprite_.reset(KetaEngine::Sprite::Create("GameOver.png"));
    } else {
      clearSprite_.reset(KetaEngine::Sprite::Create("Clear.png"));
    }
    clearSprite_->transform_.pos = Vector2(640, -720);

    // イージング初期化
    finishSpriteEase_.Init("finishSpriteTest.json");
    finishSpriteEase_.SetAdaptValue(&tempSpritePos_);
    finishSpriteEase_.Reset();

    finishSpriteEase_.SetOnFinishCallback([this]() {
        CheckEndInput();
    });
}

void GameSceneFinish::Update([[maybe_unused]] float timeSpeed) {
    // スプライト更新
    pOwner_->GetGameObj().screenSprite_->SetAlpha(alpha_);

    finishSpriteEase_.Update(timeSpeed);
    clearSprite_->transform_.pos = tempSpritePos_;

    if (!isStartFadeOut_) {
        return;
    }

    // フェードアウト
    alpha_ += timeSpeed;

    // タイトルへ遷移
    if (alpha_ >= 1.0f) {
        isGameEnd_ = true;
 
    }
}

void GameSceneFinish::CheckEndInput() {
    if (KetaEngine::Input::GetInstance()->PushKey(KeyboardKey::Space) || KetaEngine::Input::IsTriggerPad(0, GamepadButton::A)) {
        isStartFadeOut_ = true;
    }
}

void GameSceneFinish::Debug() {
}