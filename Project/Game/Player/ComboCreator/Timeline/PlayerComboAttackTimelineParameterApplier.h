#pragma once

#include "PlayerComboAttackTimelineData.h"
#include "utility/TimeLine/TimelineDrawer.h"

class PlayerComboAttackData;

/// <summary>
/// タイムラインの変更をパラメータに適用するクラス
/// </summary>
class PlayerComboAttackTimelineParameterApplier {
public:
    PlayerComboAttackTimelineParameterApplier()  = default;
    ~PlayerComboAttackTimelineParameterApplier() = default;

    void Init(PlayerComboAttackData* attackData,
        KetaEngine::TimelineDrawer* timeline,
        PlayerComboAttackTimelineData* data);

    // パラメータ適用
    void ApplyToParameters();

    // 終了待機時間の位置を更新
    void UpdateFinishWaitKeyFramePosition();

    // タイムライン終了フレームを更新
    void UpdateTimelineEndFrame();

private:
    void ApplyTrackToRendition(const PlayerComboAttackTimelineData::TrackInfo& trackInfo, float timing);
    int32_t GetFinishWaitStartFrame() const;
    int32_t CalculateTotalFrames() const;

private:
    PlayerComboAttackData* attackData_           = nullptr;
    KetaEngine::TimelineDrawer* timeline_        = nullptr;
    PlayerComboAttackTimelineData* timeLineData_ = nullptr;
};