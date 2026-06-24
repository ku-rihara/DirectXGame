#pragma once

#include "BaseGameSceneState.h"
#include "PauseController/PauseController.h"

/// <summary>
/// ゲームシーン：プレイ中ステート
/// </summary>
class GameScenePlaying : public BaseGameSceneState {
public:
    GameScenePlaying(GameScene* gameScene);
    ~GameScenePlaying() override = default;

    void Init() override;
    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

private:
    /// ポーズボタンが押されたか判定
    bool CheckPauseTransition();

    PauseController pauseController_;
    float elapsedTimeSec_ = 0.f;   ///< プレイ経過時間
    bool gameOverHandled_ = false; ///< ゲームオーバー後処理済みフラグ
};