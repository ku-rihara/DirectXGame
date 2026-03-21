#pragma once

#include "2d/Sprite.h"
#include "BaseGameSceneState.h"
#include "PauseController/PauseController.h"
#include <memory>

/// <summary>
/// ゲームシーン：ポーズステート
/// </summary>
class GameScenePose : public BaseGameSceneState {
public:
    GameScenePose(GameScene* gameScene);
    ~GameScenePose() override = default;

    void Init() override;
    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

private:
    std::unique_ptr<KetaEngine::Sprite> poseSprite_;
    PauseController pauseController_;
};
