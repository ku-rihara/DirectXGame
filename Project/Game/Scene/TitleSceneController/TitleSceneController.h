#pragma once
// 2D
#include "2d/Sprite.h"
// objects
#include "Player/Player.h"
#include "UI/TitleLogo.h"
// TitleSceneTransition
#include "TitleSceneTransition.h"
// std
#include <memory>

/// <summary>
/// タイトルシーンの演出・遷移の制御クラス
/// </summary>
class TitleSceneController {
public:
    TitleSceneController()  = default;
    ~TitleSceneController() = default;

    void Init(Player* player);
    void Update();

    // 遷移関連
    bool IsTransitionComplete() const;
    const std::unique_ptr<KetaEngine::Sprite>& GetScreenSprite() const;

private:
    void UpdatePlayerPerformance();
    void UpdateLogoSequence();

    Player* player_ = nullptr;
    std::unique_ptr<TitleLogo> titleLogo_;
    std::unique_ptr<TitleSceneTransition> sceneTransition_;
    bool isTitleLogoUpdate_ = false;
};