#pragma once

#include "BaseTitleBehavior.h"
#include "Easing/Easing.h"

/// <summary>
/// タイトル右パンチ振る舞いクラス
/// </summary>
class TitleRightPunch : public BaseTitleBehavior {
public:
    // コンストラクタ
    TitleRightPunch(Player* player);
    ~TitleRightPunch();

    void Update() override; //< 更新
    void Debug() override; //< デバッグ
    void EasingInit(); //< イージングの初期化

private:
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
    Vector3 rHandStartPos_; /// ハンドスタート座標
    Vector3 rHandTargetPos_; /// ハンドターゲット座標
    Vector3 punchPosition_;

    Easing<Vector3> scalingEase_;
    Easing<Vector3> punchEase_;
    Easing<Vector3> backPunchEase_;

    Vector3 tempScale_;

    float waitTine_; /// 次コンボまでの待機時間
};