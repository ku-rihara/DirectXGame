#pragma once

#include "BaseComboAattackBehavior.h"
#include "CollisionBox/AttackCollisionBox.h"
#include"EasingFunction.h"

class RoolingPunchAttack : public BaseComboAattackBehavior {
private:
    /// 　振る舞いオーダー
    enum class Order {
        START,
        ROOLING,
        WAIT,
        END,
    };

private:
    /// ===================================================
    /// private varians
    /// ===================================================

    Order order_; /// 振る舞い順序

    // collision
    std::unique_ptr<AttackCollisionBox> collisionBox_;

    /// パンチ
    Easing punchEase_; /// パンチイージング

    Vector3 lHandStartPos_;
    Vector3 lHandTargetPos_;

    Vector3 punchPosition_;

    float waitTine_;

public:
    // コンストラクタ
    RoolingPunchAttack(Player* player);
    ~RoolingPunchAttack();

    /// 更新、デバッグ
    void Update() override;
    void Debug() override;
    void RotateMotion();
};
