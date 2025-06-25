#pragma once

#include "BaseComboAattackBehavior.h"
#include "CollisionBox/AttackCollisionBox.h"
#include "EasingFunction.h"

class RightJobPunch : public BaseComboAattackBehavior {
public:
    // コンストラクタ
    RightJobPunch(Player* player);
    ~RightJobPunch();

    /// 更新、デバッグ
    void Update() override;
    void Debug() override;
    void CollisionBoxInit();
    void EasingInit();

    void ChangeSpeedForLockOn();

private:
    enum class Order {
        RUSH,
        PUNCH,
        BACKPUNCH,
        WAIT,
    };

private:
    /// ===================================================
    /// private varians
    /// ===================================================

    Order order_; /// 振る舞い順序

    // collision
    std::unique_ptr<AttackCollisionBox> collisionBox_;

    /// 突進
    Vector3 initPos_; /// スタート座標
    Vector3 rushPos_; /// 突進座標
    Vector3 forwardDirection_; /// 向き
    float speed_;

    /// パンチ
    Easing<Vector3> rushEase_;
    Vector3 tempRushPos_; 
    Easing<Vector3> punchEase_; /// パンチイージング
    Easing<Vector3> backPunchEase_; /// パンチイージング

    Vector3 rHandStartPos_; /// ハンドスタート座標
    Vector3 rHandTargetPos_; /// ハンドターゲット座標
    Vector3 punchPosition_;

    float waitTine_; /// 次コンボまでの待機時間
};
