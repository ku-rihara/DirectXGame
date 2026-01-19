#pragma once

#include "utility/TimeLine/TimeLine.h"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

class PlayerComboAttackData;

/// <summary>
/// タイムラインのトラック情報を管理するクラス
/// </summary>
class PlayerComboAttackTimelineManager {
public:
    enum class PlayMode {
        SINGLE,
        CONTINUOUS
    };

    enum class DefaultTrack {
        COLLISION,
        MOVE_EASING,
        CANCEL_START,
        PRECEDE_INPUT_START,
        FINISH_WAIT,
        COUNT
    };

    enum class TrackType {
        // Rendition系
        CAMERA_ACTION,
        HIT_STOP,
        SHAKE_ACTION,
        POST_EFFECT,
        PARTICLE_EFFECT,
        // Rendition On Hit系
        CAMERA_ACTION_ON_HIT,
        HIT_STOP_ON_HIT,
        SHAKE_ACTION_ON_HIT,
        POST_EFFECT_ON_HIT,
        PARTICLE_EFFECT_ON_HIT,
        // Object Animation系
        OBJ_ANIM_HEAD,
        OBJ_ANIM_RIGHT_HAND,
        OBJ_ANIM_LEFT_HAND,
        OBJ_ANIM_MAIN_HEAD,
        // Audio系
        AUDIO_ATTACK,
        AUDIO_HIT,
        // Timing系
        CANCEL_TIME,
        PRECEDE_INPUT,
        FINISH_WAIT_TIME,
        COUNT
    };

    struct TrackInfo {
        TrackType type;
        int32_t trackIndex;
        std::string fileName;
        bool isCameraReset = false;
    };

public:
    PlayerComboAttackTimelineManager()  = default;
    ~PlayerComboAttackTimelineManager() = default;

    void Init(PlayerComboAttackData* attackData, KetaEngine::TimeLine* timeline);

    // トラック管理
    void SetupDefaultTracks();
    void SetupRenditionTracks();
    void SetupObjectAnimationTracks();
    void SetupAudioTracks();

    void AddTrack(TrackType type);
    void RemoveTrack(int32_t trackIndex);

    // パラメータ適用
    void ApplyToParameters();

    // ヘルパー
    const char* GetTrackTypeName(TrackType type) const;
    TrackType GetTrackTypeFromIndex(int32_t trackIndex) const;
    std::string GetDirectoryForTrackType(TrackType type) const;
    int32_t CalculateTotalFrames() const;

    // トラック存在チェック
    bool IsTrackTypeAlreadyAdded(TrackType type) const;

    // ゲッター
    const std::vector<TrackInfo>& GetAddedTracks() const { return addedTracks_; }
    const std::array<int32_t, static_cast<size_t>(DefaultTrack::COUNT)>& GetDefaultTrackIndices() const {
        return defaultTrackIndices_;
    }

    TrackInfo* FindTrackInfo(int32_t trackIndex);

private:
    void ApplyTrackToRendition(const TrackInfo& trackInfo, float timing);

private:
    PlayerComboAttackData* attackData_ = nullptr;
    KetaEngine::TimeLine* timeline_    = nullptr;

    std::array<int32_t, static_cast<size_t>(DefaultTrack::COUNT)> defaultTrackIndices_;
    std::vector<TrackInfo> addedTracks_;
};