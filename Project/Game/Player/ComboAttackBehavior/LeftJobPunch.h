#pragma once

#include "BaseComboAattackBehavior.h"
#include "CollisionBox/PlayerAttackController.h"
#include "Easing.h"
#include "EasingFunction.h"

class LeftJobPunch : public BaseComboAattackBehavior {
public:
    // コンストラクタ
    LeftJobPunch(Player* player);
    ~LeftJobPunch();

    /// 更新、デバッグ
    void Update() override;
    void Debug() override;
    void CollisionBoxInit();
    void EasingInit();

private:
    /// 　振る舞いオーダー
    enum class Order {
        PUNCH,
        BACKPUNCH,
        WAIT,
    };

private:
    /// ===================================================
    /// private varians
    /// ===================================================

    Order order_; /// 振る舞い順序

    /// パンチ
    Easing<Vector3> punchEase_; /// パンチイージング
    Easing<Vector3> backPunchEase_; /// パンチイージング

    Vector3 lHandStartPos_; /// ハンドスタート座標
    Vector3 lHandTargetPos_; /// ハンドターゲット座標
    Vector3 punchPosition_; /// パンチ位置

    float waitTine_; /// 次コンボまでの待機時間

};
