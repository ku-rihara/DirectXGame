#pragma once

#include "BaseTitleBehavior.h"

/// <summary>
/// タイトル用プレイヤー振る舞いクラス
/// </summary>
class TitlePlayerBehavior : public BaseTitleBehavior {
public:
    TitlePlayerBehavior(Player* player);
    ~TitlePlayerBehavior() = default;

    void Update() override; //< 更新
    void Debug() override;  //< デバッグ

private:
    bool isFinish_ = false;

    float elapsedTime_              = 0.0f;
    bool hasLandingParticleEmitted_ = false;

public:
    bool GetIsFinish() const { return isFinish_; }
};
