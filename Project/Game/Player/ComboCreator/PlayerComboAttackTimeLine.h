#pragma once

#include "utility/TimeLine/TimeLine.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class PlayerComboAttackData;

/// <summary>
/// プレイヤー攻撃用タイムラインエディタ
/// </summary>
class PlayerComboAttackTimeline {
public:
    enum class PlayMode {
        SINGLE, // 単体再生
        CONTINUOUS // 連続再生
    };

    enum class DefaultTrack {
        COLLISION,
        MOVE_EASING,
        CANCEL_START,
        PRECEDE_INPUT_START,
        COUNT
    };

    // 追加可能なトラックタイプ
    enum class AddableTrackType {
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
        COUNT
    };

    struct TrackInfo {
        AddableTrackType type;
        int32_t trackIndex;
        std::string fileName; // 演出系で使用
    };

public:
    PlayerComboAttackTimeline()  = default;
    ~PlayerComboAttackTimeline() = default;

    void Init(PlayerComboAttackData* attackData);
    void Draw();
    void ApplyToParameters();

private:
    // セットアップ関連
    void SetupDefaultTracks();
    void SetupRenditionTracks();
    void SetupObjectAnimationTracks();
    void SetupAudioTracks();

    // トラック追加UI
    void DrawAddTrackButton();
    void DrawAddTrackPopup();

    // パラメータ編集ボタン
    void DrawParamEditButtons();

    // トラック右クリックメニュー
    void DrawTrackContextMenu(int32_t trackIndex);

    // キーフレーム右クリックメニュー
    void DrawKeyFrameContextMenu(int32_t trackIndex, int32_t keyIndex);

    // パラメータ編集UI
    void DrawCollisionParamUI();
    void DrawMoveParamUI();
    void DrawTimingParamUI(AddableTrackType type);
    void DrawRenditionFileSelector(AddableTrackType type, TrackInfo& info);
    void DrawRenditionKeyFrameEditor(int32_t trackIndex, int32_t keyIndex);

    // ヘルパー関数
    int32_t CalculateTotalFrames() const;
    void AdvanceToNextAttack();
    const char* GetTrackTypeName(AddableTrackType type) const;
    AddableTrackType GetTrackTypeFromIndex(int32_t trackIndex);
    std::string GetDirectoryForTrackType(AddableTrackType type) const;

    // トラック追加・削除
    void AddTrack(AddableTrackType type);
    void RemoveTrack(int32_t trackIndex);

    // Apply機能
    void ApplyTrackToRendition(const TrackInfo& trackInfo, float timing);

private:
    PlayerComboAttackData* attackData_ = nullptr;
    KetaEngine::TimeLine timeline_;

    // デフォルトトラックインデックス管理
    std::array<int32_t, static_cast<size_t>(DefaultTrack::COUNT)> defaultTrackIndices_;

    // 追加されたトラック情報
    std::vector<TrackInfo> addedTracks_;

    // UI状態
    PlayMode playMode_  = PlayMode::SINGLE;
    bool isInitialized_ = false;

    // 選択中のパラメータ編集タイプ
    int32_t selectedParamEditType_ = -1; 
};