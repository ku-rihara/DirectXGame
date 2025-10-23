#pragma once

#include "BaseTitleBehavior.h"
#include "Easing/Easing.h"

/// <summary>
/// タイトル左パンチ振る舞いクラス
/// </summary>
class TitleLeftPunch : public BaseTitleBehavior {
public:
    // コンストラクタ
    TitleLeftPunch(Player* player);
    ~TitleLeftPunch();

    void Update() override; //< 更新
    void Debug() override; //< デバッグ
    void EasingInit(); //< イージングの初期化

private:
    /// 　振る舞いオーダー
    enum class Order {
        PUNCH,
        BACKPUNCH,
        WAIT,
    };

private:
    /// ===================================================
    /// private variable
    /// ===================================================

    Order order_; /// 振る舞い順序

    /// パンチ
    Easing<Vector3> scalingEase_;
    Easing<Vector3> punchEase_;
    Easing<Vector3> backPunchEase_;

    Vector3 tempScale_;

    Vector3 lHandStartPos_;  /// ハンドスタート座標
    Vector3 lHandTargetPos_; /// ハンドターゲット座標
    Vector3 punchPosition_;  /// パンチ位置

    float waitTine_; /// 次コンボまでの待機時間
};