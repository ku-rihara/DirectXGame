#pragma once

#include "BaseTitleBehavior.h"
// std
#include <functional>
#include <unordered_map>

/// <summary>
/// タイトル用プレイヤー振る舞いクラス
/// </summary>
class TitlePlayerBehavior : public BaseTitleBehavior {
public:
    TitlePlayerBehavior(Player* player);
    ~TitlePlayerBehavior() = default;

    // 更新、デバッグ
    void Update() override;
    void Debug() override;

    // フェーズごとのアニメーションのセットアップ
    void SetUpAnimations();

private:
    enum class Phase {
        FALL, // 落下
        LAND, // 着地演出
        LEFT_PUNCH, // 左パンチ
        RIGHT_PUNCH, // 右パンチ
        WAITING, // 待機ループ
    };

private:
    // フェーズごとのアニメーション関数
    void UpdateFall();
    void UpdateLand();
    void UpdateLeftPunch();
    void UpdateRightPunch();
    void UpdateWaiting();

private:
    // 終了フラグとフェーズ
    bool isFinish_ = false;
    Phase phase_   = Phase::FALL;

    // フェーズごとの更新処理
    std::unordered_map<Phase, std::function<void()>> animationPhase_;

public:
    // Getter
    bool GetIsFinish() const { return isFinish_; }
};
