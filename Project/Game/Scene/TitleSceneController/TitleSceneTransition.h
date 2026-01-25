// TitleSceneTransition.h
#pragma once
#include "2d/Sprite.h"
#include "input/Input.h"
#include <memory>

/// <summary>
/// タイトルシーンの遷移管理クラス
/// </summary>
class TitleSceneTransition {
public:
    TitleSceneTransition()  = default;
    ~TitleSceneTransition() = default;

    void Init();
    void Update();

private:
    void CheckInput();
    void UpdateFadeOut();

    std::unique_ptr<KetaEngine::Sprite> screenSprite_;
    bool isTransitionRequested_ = false;
    float transitionAlpha_      = 0.0f;

public:
    bool IsTransitionRequested() const { return isTransitionRequested_; }
    bool IsTransitionComplete() const { return transitionAlpha_ >= 1.2f; }
    float GetAlpha() const { return transitionAlpha_; }

    const std::unique_ptr<KetaEngine::Sprite>& GetScreenSprite() const {
        return screenSprite_;
    }
};