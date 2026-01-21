#pragma once

#include "PlayerComboAttackTimelineData.h"
#include "utility/TimeLine/TimeLine.h"

class PlayerComboAttackData;

/// <summary>
/// タイムラインのトラックを構築するクラス
/// </summary>
class PlayerComboAttackTimelineTrackBuilder {
public:
    PlayerComboAttackTimelineTrackBuilder()  = default;
    ~PlayerComboAttackTimelineTrackBuilder() = default;

    void Init(PlayerComboAttackData* attackData,
        KetaEngine::TimeLine* timeline,
        PlayerComboAttackTimelineData* data);

    // トラックセットアップ
    void SetupDefaultTracks();
    void SetupRenditionTracks();
    void SetupObjectAnimationTracks();
    void SetupAudioTracks();

    // トラックの追加・削除
    void AddTrack(PlayerComboAttackTimelineData::TrackType type);
    void RemoveTrack(int32_t trackIndex);

    // タイムライン終了フレーム計算
    int32_t CalculateTotalFrames() const;

private:
    PlayerComboAttackData* attackData_   = nullptr;
    KetaEngine::TimeLine* timeline_      = nullptr;
    PlayerComboAttackTimelineData* data_ = nullptr;
};