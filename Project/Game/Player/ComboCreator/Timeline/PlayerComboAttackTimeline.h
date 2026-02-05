#pragma once

// timeline
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
    void Update(float deltaTime);
    void Draw();
    void ApplyToParameters();
    void RebuildBranchTracks();

private:
    void SetupKeyFrameCallbacks();

private:
    PlayerComboAttackData* attackData_ = nullptr;
    KetaEngine::TimelineDrawer timeline_;

    // 各責務を持つクラス
    PlayerComboAttackTimelineData timelineData_;
    PlayerComboAttackTimelineTrackBuilder trackBuilder_;
    PlayerComboAttackTimelineParameterApplier parameterApplier_;
    PlayerComboAttackTimelineUI ui_;

    bool isInitialized_ = false;

public:
    // Getter
    KetaEngine::TimelineDrawer* GetTimeline();
};