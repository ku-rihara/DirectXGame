#pragma once
#include "../../ComboCreator/ComboBranchParameter.h"
#include "../../ComboCreator/PlayerAttackRenditionData.h"
#include "BaseComboAttackBehavior.h"
#include "Easing/Easing.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Rendition/PlayerAttackRendition.h"
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class PlayerAttackCollider;

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

    // 次の攻撃候補
    struct NextAttackCandidate {
        ComboBranchParameter* branch;
        PlayerComboAttackData* attackData;
    };

    const std::vector<NextAttackCandidate>& GetNextAttackCandidates() const { return nextAttackCandidates_; }

    /// ロック中を含む全分岐候補
    const std::vector<NextAttackCandidate>& GetAllNextAttackCandidates() const { return allNextAttackCandidates_; }

private:
    /// 振る舞いオーダー
    enum class Order {
        INIT,
        PREP_ATTACK,   // 予備動作
        ATTACK,
        FINISH_ATTACK, // 終了処理
        WAIT,
        CHANGE,
    };

    void SetOrder(Order order);

    void InitializeAttack();
    void UpdatePrepAttack(float atkSpeed);
    void UpdateAttack(float atkSpeed);
    void UpdateFinishAttack(float atkSpeed);
    void UpdateWait(float atkSpeed);
    void ChangeNextAttack();
    void ApplyMovement(float atkSpeed);
    void SetupCollision(AttackTimelinePhase phase = AttackTimelinePhase::MAIN);
    void SetMoveEasing();
    void SetPrepMoveEasing();
    void SetFinishMoveEasing();

    // 共通イージングセットアップ: フェーズのパラメータから start/target を計算し easing と currentValue を設定する
    std::pair<Vector3, Vector3> SetPhaseEasing(AttackTimelinePhase phase, KetaEngine::Easing<Vector3>& easing, Vector3& currentValue);
    void FaceTowardTarget(const Vector3& targetPos);

    void PreOderNextComboForButton();
    void TryAutoSelectNextFromQueue(); // キューから次の攻撃を自動選択
    void AttackCancel();

    bool IsAttackUnlock(const PlayerComboAttackData&data) const;

    // 移動経路上の最も手前にいる敵の目の前の座標を返す
    Vector3 CalcStopBeforeEnemyTarget(
        const Vector3& start, const Vector3& defaultTarget,
        Vector3* outFoundEnemyPos = nullptr) const;

private:
    Order order_;
    std::function<void(float)> orderFunc_;
    PlayerComboAttackData* attackData_        = nullptr;
    PlayerAttackCollider* pCollisionInfo_ = nullptr;

    // 次の攻撃候補リスト（ロック解除済みのみ）
    std::vector<NextAttackCandidate> nextAttackCandidates_;
    // ロック中を含む全分岐候補
    std::vector<NextAttackCandidate> allNextAttackCandidates_;
    int32_t selectedBranchIndex_ = -1; 

    // タイミング
    float currentFrame_;
    float waitTime_;

    bool isReserveNextCombo_;
    bool isAutoReservedCombo_;
    bool isAttackCancel_;
    bool hasHitEnemy_;
    int32_t autoSelectedBranchIndex_ = -1; 

    // 演出
    std::unique_ptr<PlayerAttackRendition> attackRendition_;
    std::unique_ptr<PlayerAttackRendition> prepRendition_;
    std::unique_ptr<PlayerAttackRendition> finishRendition_;

    // メインフェーズ移動
    KetaEngine::Easing<Vector3> moveEasing_;
    Vector3 currentMoveValue_;
    Vector3 startPosition_;
    Vector3 targetPosition_;

    // 予備動作フェーズ移動
    KetaEngine::Easing<Vector3> prepMoveEasing_;
    Vector3 prepCurrentMoveValue_;

    // 終了処理フェーズ移動
    KetaEngine::Easing<Vector3> finishMoveEasing_;
    Vector3 finishCurrentMoveValue_;

    // コリジョン
    bool isCollisionActive_;
    float collisionTimer_;

    // ATTACKフェーズの生時間カウンタ
    // 長時間スタック時の安全脱出用
    float attackRawTimer_                   = 0.0f;
    static constexpr float kAttackTimeout   = 5.0f;
};