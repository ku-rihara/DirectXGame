#pragma once
// 2D
#include "2d/Sprite.h"
// objects
#include "Player/TitlePlayer/TitlePlayer.h"
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

    // 初期化、更新
    void Init(TitlePlayer* player);
    void Update();

    // 遷移関連
    bool IsTransitionComplete() const;
 
private:
    /// <summary>
    /// タイトルのプレイヤーの演出を更新
    /// </summary>
    void UpdatePlayerPerformance();

    /// <summary>
    /// タイトルロゴの遷移を更新
    /// </summary>
    void UpdateLogoSequence();

private:
    TitlePlayer* player_ = nullptr;
    std::unique_ptr<TitleLogo> titleLogo_;
    std::unique_ptr<TitleSceneTransition> sceneTransition_;
    bool isTitleLogoUpdate_ = false;
};