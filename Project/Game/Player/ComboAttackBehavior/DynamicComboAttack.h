#pragma once
#include"Rendition/PlayerAttackRendition.h"
#include "BaseComboAattackBehavior.h"
#include "Easing/Easing.h"
#include <memory>

/// <summary>
/// データ駆動型のコンボ攻撃クラス
/// </summary>
class DynamicComboAttack : public BaseComboAattackBehavior {
public:
    DynamicComboAttack(Player* player, PlayerComboAttackData* attackData);
    ~DynamicComboAttack();

    void Init()   override;
    void Update() override;
    void Debug()  override;

private:
    /// 振る舞いオーダー
    enum class Order {
        INIT,
        ATTACK,
        RECOVERY,
        WAIT,
    };

    void InitializeAttack();
    void UpdateAttack();
    void UpdateRecovery();
    void UpdateWait();
    void ApplyMovement();
    void SetupCollision();

private:
    Order order_;
    PlayerComboAttackData* attackData_;

    // タイミング
    float currentFrame_;
    float waitTime_;

    // 移動関連
    std::unique_ptr<PlayerAttackRendition> attackRendition_;
    Easing<Vector3> moveEasing_;
    Vector3 currentMoveValue_;
    Vector3 startPosition_;
    Vector3 targetPosition_;

    // コリジョン
    bool isCollisionActive_;
    float collisionTimer_;
};