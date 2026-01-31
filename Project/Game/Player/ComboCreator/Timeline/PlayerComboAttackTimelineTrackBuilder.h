#pragma once

#include "PlayerComboAttackTimelineData.h"
#include "utility/TimeLine/TimelineDrawer.h"
#include <string>

class PlayerComboAttackData;

/// <summary>
/// タイムラインのトラックを構築するクラス
/// </summary>
class PlayerComboAttackTimelineTrackBuilder {
public:
    PlayerComboAttackTimelineTrackBuilder()  = default;
    ~PlayerComboAttackTimelineTrackBuilder() = default;

    void Init(PlayerComboAttackData* attackData,
        KetaEngine::TimelineDrawer* timeline,
        PlayerComboAttackTimelineData* data);

    // トラックセットアップ
    void SetupDefaultTracks();
    void SetupRenditionTracks();
    void SetupObjectAnimationTracks();
    void SetupAudioTracks();

    // トラックの追加・削除
    void AddTrack(PlayerComboAttackTimelineData::TrackType type);
    void RemoveTrack(int32_t trackIndex);

    // コンボ分岐トラックの再構築
    void RebuildBranchTracks();

    // タイムライン終了フレーム計算
    int32_t CalculateTotalFrames() const;

private:


    std::string GetButtonDisplayName(int32_t keyboardButton, int32_t gamepadButton);

private:

    PlayerComboAttackData* attackData_          = nullptr;
    KetaEngine::TimelineDrawer* timelineDrawer_ = nullptr;
    PlayerComboAttackTimelineData* data_        = nullptr;
};