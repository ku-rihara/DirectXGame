#include "GameSceneFinish.h"
#include "base/TextureManager.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"
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
    isWaitingInput_ = false;

    // スプライト初期化
    if (pOwner_->GetGameObj().player_->GetIsDeathRenditionFinish()) {
        clearSprite_.reset(KetaEngine::Sprite::Create("GameOver.dds"));
    } else {
      clearSprite_.reset(KetaEngine::Sprite::Create("Clear.dds"));
    }

    // イージングアニメーション再生
    clearSprite_->PlaySpriteEaseAnimation("finishSpritePos", "GameSceneFinish");
}

void GameSceneFinish::Update([[maybe_unused]] float timeSpeed) {
    // スプライト更新
    pOwner_->GetGameObj().screenSprite_->SetAlpha(alpha_);

    // アニメーション終了判定（コールバックの代替）
    if (!isWaitingInput_ && clearSprite_->GetSpriteEaseAnimationPlayer()->IsFinished()) {
        isWaitingInput_ = true;
    }

    if (isWaitingInput_) {
        CheckEndInput();
    }

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
