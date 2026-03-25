#pragma once

#include "2d/Sprite.h"
#include "BaseGameSceneState.h"
#include "Easing/Easing.h"
// std
#include <memory>

/// <summary>
/// ゲームシーン：終了ステート
/// </summary>
class GameSceneFinish : public BaseGameSceneState {
public:
    GameSceneFinish(GameScene* gameScene);
    ~GameSceneFinish() override = default;

    void Init() override;
    void Update([[maybe_unused]] float timeSpeed = 0.0f) override;
    void Debug() override;

private:
    enum class Phase {
        kSpriteFallIn, // 上から降ってくるアニメーション待ち
        kFadeIn,       // 画面フェードイン（暗転）
        kDone,
    };

    Phase phase_ = Phase::kSpriteFallIn;
    float alpha_ = 0.7f;

    // 上から降ってくるクリアスプライト
    std::unique_ptr<KetaEngine::Sprite> clearSprite_;

    // フェードイン用イージング（0.7 → 1.0）
    KetaEngine::Easing<float> fadeEasing_;
};
