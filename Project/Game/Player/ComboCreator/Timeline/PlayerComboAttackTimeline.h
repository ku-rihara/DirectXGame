#pragma once


// Timeline
#include "PlayerComboAttackTimelineData.h"
#include "PlayerComboAttackTimelineParameterApplier.h"
#include "PlayerComboAttackTimelineTrackBuilder.h"
#include "PlayerComboAttackTimelineUI.h"
// utility
#include "utility/TimeLine/TimelineDrawer.h"
// std
#include <memory>

class PlayerComboAttackData;

/// <summary>
/// プレイヤー攻撃用タイムラインエディタ
/// </summary>
class PlayerComboAttackTimeline {
public:
    PlayerComboAttackTimeline()  = default;
    ~PlayerComboAttackTimeline() = default;

    void Init(PlayerComboAttackData* attackData);
    void Draw();
    void ApplyToParameters();

private:
    void AdvanceToNextAttack();
    void SetupKeyFrameCallbacks();

private:
    PlayerComboAttackData* attackData_ = nullptr;
    KetaEngine::TimelineDrawer timeline_;

    PlayerComboAttackTimelineData data_;
    PlayerComboAttackTimelineTrackBuilder trackBuilder_;
    PlayerComboAttackTimelineParameterApplier parameterApplier_;
    PlayerComboAttackTimelineUI ui_;

    bool isInitialized_ = false;

public:
    // プレビュー状態の確認用
    bool IsPreviewPlaying() const;
};