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
    /// <summary>
    /// ポーズボタンが押されたか判定し、ポーズステートへ移行する
    /// </summary>
    /// <returns>ポーズボタンがトリガーされた場合 true</returns>
    bool CheckPauseTransition();

    PauseController pauseController_;
};