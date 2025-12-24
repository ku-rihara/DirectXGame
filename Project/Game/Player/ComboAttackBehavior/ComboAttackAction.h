#pragma once
#include "BaseComboAttackBehavior.h"
#include "Easing/Easing.h"
#include "Rendition/PlayerAttackRendition.h"
#include <memory>
#include <string>

class PlayerCollisionInfo;

/// <summary>
/// データ駆動型のコンボ攻撃クラス
/// </summary>
class ComboAttackAction : public BaseComboAttackBehavior {
public:
    ComboAttackAction(Player* player, PlayerComboAttackData* attackData);
    ~ComboAttackAction();

    void Init() override;
    void Update(float atkSpeed) override;
    void Debug() override;

private:
    /// 振る舞いオーダー
    enum class Order {
        INIT,
        ATTACK,
        WAIT,
        CHANGE,
    };

    void InitializeAttack();
    void UpdateAttack(float atkSpeed);
    void UpdateWait(float atkSpeed);
    void ChangeNextAttack();
    void ApplyMovement(float atkSpeed);
    void SetupCollision();
    void SetMoveEasing();

    void PreOderNextComboForButton();
    void AttackCancel();

private:
    Order order_;
    PlayerComboAttackData* attackData_     = nullptr;
    PlayerComboAttackData* nextAttackData_ = nullptr;
    PlayerCollisionInfo* pCollisionInfo_   = nullptr;

   
    // タイミング
    float currentFrame_;
    float waitTime_;

    bool isReserveNextCombo_;
    bool isAttackCancel_;
    bool hasHitEnemy_; 

    // 移動関連
    std::unique_ptr<PlayerAttackRendition> attackRendition_;
    KetaEngine::Easing<Vector3> moveEasing_;
    Vector3 currentMoveValue_;
    Vector3 startPosition_;
    Vector3 targetPosition_;

    std::string nextAttackName_;

    // コリジョン
    bool isCollisionActive_;
    float collisionTimer_;
};