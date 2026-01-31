#pragma once
#include "../../ComboCreator/ComboBranchParameter.h"
#include "../../ComboCreator/PlayerAttackRenditionData.h"
#include "BaseComboAttackBehavior.h"
#include "Easing/Easing.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Rendition/PlayerAttackRendition.h"
#include <memory>
#include <string>
#include <vector>

class PlayerAttackCollisionBox;

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

    // 次の攻撃候補（分岐と攻撃データのペア）
    struct NextAttackCandidate {
        ComboBranchParameter* branch;
        PlayerComboAttackData* attackData;
    };

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
    PlayerComboAttackData* attackData_        = nullptr;
    PlayerAttackCollisionBox* pCollisionInfo_ = nullptr;

    // 次の攻撃候補リスト
    std::vector<NextAttackCandidate> nextAttackCandidates_;
    int32_t selectedBranchIndex_ = -1; // 選択された分岐のインデックス

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

    // コリジョン
    bool isCollisionActive_;
    float collisionTimer_;
};