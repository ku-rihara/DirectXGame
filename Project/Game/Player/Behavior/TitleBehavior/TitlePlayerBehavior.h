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
    enum class Phase {
        FALL,        // TitlePlayer  落下
        LAND,        // TitlePlayerLand  着地演出
        LEFT_PUNCH,  // TitleLeftHand : 左パンチ
        RIGHT_PUNCH, // TitleRightHand : 右パンチ
        WAITING,     // TitleWaiting : 待機ループ
    };

    Phase phase_   = Phase::FALL;
    bool isFinish_ = false;

public:
    bool GetIsFinish() const { return isFinish_; }
};
