#pragma once

#include "2d/Sprite.h"
#include "BaseGameSceneState.h"
#include "Easing/Easing.h"
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
    enum class Phase {
        kSpriteAnim, // TimeUpスプライトアニメーション待ち
        kFadeIn,     // 画面フェードイン（暗転）
        kDone,
    };

    Phase phase_ = Phase::kSpriteAnim;
    float alpha_ = 0.7f;

    std::unique_ptr<KetaEngine::Sprite> gameOverSprite_;

    KetaEngine::Easing<float> fadeEasing_;
};
