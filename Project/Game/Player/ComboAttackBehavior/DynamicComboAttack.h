#pragma once
#include "BaseComboAattackBehavior.h"
#include "Easing/Easing.h"
#include "Rendition/PlayerAttackRendition.h"
#include <memory>
#include <string>

/// <summary>
/// データ駆動型のコンボ攻撃クラス
/// </summary>
class DynamicComboAttack : public BaseComboAattackBehavior {
public:
    DynamicComboAttack(Player* player, PlayerComboAttackData* attackData);
    ~DynamicComboAttack();

    void Init() override;
    void Update() override;
    void Debug() override;

private:
    /// 振る舞いオーダー
    enum class Order {
        INIT,
        ATTACK,
        WAIT,
    };

    void InitializeAttack();
    void UpdateAttack();
    void UpdateWait();
    void ChangeNextAttack();
    void ApplyMovement();
    void SetupCollision();

    void PreOderNextComboForButton();
    void AttackCancel();

private:
    Order order_;
    PlayerComboAttackData* attackData_     = nullptr;
    PlayerComboAttackData* nextAttackData_ = nullptr;

    // タイミング
    float currentFrame_;
    float waitTime_;

   

    bool isReserveNextCombo_;

    // 移動関連
    std::unique_ptr<PlayerAttackRendition> attackRendition_;
    Easing<Vector3> moveEasing_;
    Vector3 currentMoveValue_;
    Vector3 startPosition_;
    Vector3 targetPosition_;

    std::string nextAttackName_;

    // コリジョン
    bool isCollisionActive_;
    float collisionTimer_;
};