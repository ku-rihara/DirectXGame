#pragma once

#include "2d/Sprite.h"
#include "BaseGameSceneState.h"
#include <memory>

/// <summary>
/// ゲームシーン:イントロステート
/// </summary>
class GameSceneIntro : public BaseGameSceneState {
public:
    GameSceneIntro(GameScene* gameScene);
    ~GameSceneIntro() override = default;

    void Init() override;
    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

private:
    // イントロステート固有の変数
    float alpha_        = 1.0f;
    bool isFirstChange_ = false;

};