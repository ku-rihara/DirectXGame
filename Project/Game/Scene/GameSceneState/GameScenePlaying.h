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

    // 初期化、更新、デバッグ
    void Init() override;
    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

private:
    /// <summary>
    /// ポーズボタンが押されたか判定
    /// </summary>
    /// <returns>ポーズボタンがトリガーされた場合true</returns>
    bool CheckPauseTransition();

    PauseController pauseController_;
    float elapsedTimeSec_ = 0.f;   ///< プレイ経過時間（秒）
    bool gameOverHandled_ = false; ///< ゲームオーバー時の後処理
};