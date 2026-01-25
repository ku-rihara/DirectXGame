#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

/// <summary>
/// タイムラインのトラック情報とデータ構造を管理するクラス
/// </summary>
class PlayerComboAttackTimelineData {
public:
    enum class PlayMode {
        SINGLE,
        CONTINUOUS
    };

    enum class DefaultTrack {
        COLLISION,
        MOVE_EASING,
        FINISH_WAIT,
        CANCEL_START,
        PRECEDE_INPUT_START,
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
    PlayerComboAttackTimelineData()  = default;
    ~PlayerComboAttackTimelineData() = default;

    void Init();

    // トラック情報管理
    void AddTrackInfo(const TrackInfo& info);
    void RemoveTrackInfo(int32_t trackIndex);
    TrackInfo* FindTrackInfo(int32_t trackIndex);
    bool IsTrackTypeAlreadyAdded(TrackType type) const;

    // ヘルパー
    const char* GetTrackTypeName(TrackType type) const;
    TrackType GetTrackTypeFromIndex(int32_t trackIndex) const;
    std::string GetDirectoryForTrackType(TrackType type) const;

    // デフォルトトラックインデックス管理
    void SetDefaultTrackIndex(DefaultTrack track, int32_t index);
    int32_t GetDefaultTrackIndex(DefaultTrack track) const;

private:
    std::array<int32_t, static_cast<size_t>(DefaultTrack::COUNT)> defaultTrackIndices_;
    std::vector<TrackInfo> addedTracks_;

public:
    // getter
    const std::vector<TrackInfo>& GetAddedTracks() const { return addedTracks_; }
    const std::array<int32_t, static_cast<size_t>(DefaultTrack::COUNT)>&
    GetDefaultTrackIndices() const { return defaultTrackIndices_; }
};