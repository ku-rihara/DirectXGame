#include "TitleSceneTransition.h"
#include "base/TextureManager.h"
#include "Frame/Frame.h"

void TitleSceneTransition::Init() {
    screenSprite_.reset(KetaEngine::Sprite::Create("screenChange.png"));
    isTransitionRequested_ = false;
    transitionAlpha_       = 0.0f;
}

void TitleSceneTransition::Update() {
    // 入力チェック
    CheckInput();

    // フェードアウト処理
    UpdateFadeOut();

    // スプライトのアルファ値更新
    screenSprite_->SetAlpha(transitionAlpha_);
}

void TitleSceneTransition::CheckInput() {
    // すでに遷移開始している場合はスキップ
    if (isTransitionRequested_) {
        return;
    }

    // キーボード入力（Space）
    if (KetaEngine::Input::GetInstance()->PushKey(KeyboardKey::Space)) {
        isTransitionRequested_ = true;
        return;
    }

    // コントローラー入力（Aボタン）
    if (KetaEngine::Input::IsTriggerPad(0, GamepadButton::A)) {
        isTransitionRequested_ = true;
    }
}

void TitleSceneTransition::UpdateFadeOut() {
    if (!isTransitionRequested_)
        return;

    // アルファ値を徐々に増加
    transitionAlpha_ += KetaEngine::Frame::DeltaTime();
}