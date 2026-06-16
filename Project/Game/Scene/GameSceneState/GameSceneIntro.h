#pragma once

#include "BaseGameSceneState.h"
#include <functional>

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
    /// <summary>
    /// 暗転スプライトをフェードアウトさせるフェーズ
    /// </summary>
    /// <param name="timeSpeed">タイムスピード</param>
    void PhaseFadeIn(float timeSpeed);
    /// <summary>
    /// イントロ演出を進行するフェーズ
    /// </summary>
    /// <param name="timeSpeed">タイムスピード</param>
    void PhaseIntro(float timeSpeed);

    float alpha_            = 1.0f;    //< 暗転スプライトのアルファ値
    bool shouldTransition_  = false;   //< Playingステートへの遷移フラグ
    std::function<void(float)> phase_; //< 現在のフェーズ関数
};