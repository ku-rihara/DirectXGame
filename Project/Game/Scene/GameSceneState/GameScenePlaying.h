#pragma once

#include "BaseGameSceneState.h"

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
};