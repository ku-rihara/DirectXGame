#pragma once

#include "2d/Sprite.h"
#include "BaseGameSceneState.h"
#include "Easing/Easing.h"
#include "Vector2.h"
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
    // フィニッシュステート固有の変数
    float alpha_         = 0.0f;
    bool isStartFadeOut_ = false;
    bool isGameEnd_      = false;

    std::unique_ptr<KetaEngine::Sprite> screenSprite_;
    std::unique_ptr<KetaEngine::Sprite> clearSprite_;

    KetaEngine::Easing<Vector2> finishSpriteEase_;
    Vector2 tempSpritePos_;

    void CheckEndInput();

public:
    bool GetIsGameEnd() const { return isGameEnd_; }
};