#pragma once

#include "2D/Sprite.h"
#include "BaseGameSceneState.h"
#include "Editor/Easing/Easing.h"
#include <functional>
#include <memory>

/// <summary>
/// ゲームシーン：ゲームオーバーステート
/// </summary>
class GameSceneGameOver : public BaseGameSceneState {
public:
    GameSceneGameOver(GameScene* gameScene);
    ~GameSceneGameOver() override = default;

    void Init() override;
    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

private:
    /// <summary>
    /// TimeUpスプライトアニメーション待ちの状態
    /// </summary>
    void StateSpriteAnim(float timeSpeed);
    /// <summary>
    /// 画面フェードインの状態
    /// </summary>
    void StateFadeIn(float timeSpeed);

    std::function<void(float)> currentState_;
    float alpha_ = 0.7f;

    std::unique_ptr<KetaEngine::Sprite> gameOverSprite_;

    KetaEngine::Easing<float> fadeEasing_;
};
