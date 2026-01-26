#pragma once

#include "Player/ComboAttackBehavior/BaseComboAttackBehavior.h"
#include "Vector3.h"
#include <cstdint>

class PlayerComboAttackData;
class PlayerComboAttackController;

namespace KetaEngine {
class TimelineDrawer;
}

/// <summary>
/// タイムラインでの攻撃プレビュー機能クラス
/// </summary>
class PlayerComboAttackPreview : public BaseComboAttackBehavior {
public:
    enum class PreviewMode {
        SINGLE, // 単体再生
        CHAIN, // 連続再生
    };

public:
    PlayerComboAttackPreview(
        Player* player,
        PlayerComboAttackData* startAttackData,
        PreviewMode mode,
        KetaEngine::TimelineDrawer* timeline);

    ~PlayerComboAttackPreview() = default;

    void Init() override;
    void Update(float atkSpeed) override;
    void Debug() override;

    // プレビュー制御用
    bool IsFinished() const { return isFinished_; }
    void RequestStop() { shouldStop_ = true; }

private:
    void UpdateSinglePreview(float atkSpeed);
    void UpdateChainPreview(float atkSpeed);
    void SimulateButtonInput();
    void AdvanceToNextAttack();
    PlayerComboAttackData* FindFirstAttackInChain();

private:
    KetaEngine::TimelineDrawer* timeline_          = nullptr;
    PlayerComboAttackData* currentAttackData_      = nullptr;
    PlayerComboAttackData* nextAttackData_         = nullptr;
    PlayerComboAttackController* attackController_ = nullptr;

    PreviewMode previewMode_;

    float currentTime_           = 0.0f;
    float buttonInputTimer_      = 0.0f;
    int32_t buttonInputInterval_ = 5;

    bool isWaitingForNextAttack_ = false;
    bool hasSimulatedInput_      = false;
    bool isFinished_             = false;
    bool shouldStop_             = false;

    // 初期位置保存
    Vector3 initialPlayerPosition_;
    Vector3 initialPlayerRotation_;
};