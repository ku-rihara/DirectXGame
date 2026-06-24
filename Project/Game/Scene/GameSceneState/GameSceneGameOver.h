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
    /// TimeUpスプライトアニメーション待ちの状態
    void StateSpriteAnim(float timeSpeed);
    /// 画面フェードインの状態
    void StateFadeIn(float timeSpeed);

    std::function<void(float)> currentState_;         ///< 現在の状態関数
    float alpha_ = 0.7f;                              ///< 暗転オーバーレイのアルファ値

    std::unique_ptr<KetaEngine::Sprite> gameOverSprite_; ///< ゲームオーバー表示スプライト

    KetaEngine::Easing<float> fadeEasing_;            ///< フェードイン用イージング
};
