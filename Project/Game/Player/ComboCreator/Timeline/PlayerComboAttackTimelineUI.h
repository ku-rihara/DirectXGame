#pragma once

#include "PlayerComboAttackTimelineData.h"
#include "utility/FileSelector/FileSelector.h"
#include <cstdint>
#include <unordered_map>
#include <functional>

class PlayerComboAttackData;
class PlayerComboAttackTimelineTrackBuilder;

namespace KetaEngine {
class TimelineDrawer;
}

/// <summary>
/// タイムラインのUI描画を担当するクラス
/// </summary>
class PlayerComboAttackTimelineUI {
public:
    enum class ParamEditType {
        NONE,
        COLLISION,
        MOVE,
        TRIGGER,
        FLAGS,
        NEXT_ATTACK
    };

public:
    PlayerComboAttackTimelineUI()  = default;
    ~PlayerComboAttackTimelineUI() = default;

    void Init(PlayerComboAttackData* attackData,
        KetaEngine::TimelineDrawer* timeline,
        PlayerComboAttackTimelineData* data,
        PlayerComboAttackTimelineTrackBuilder* trackBuilder);

    // メインUI描画
    void DrawParamEditButtons();
    void DrawAddTrackButton();

    // コンテキストメニュー
    void DrawTrackContextMenu(int32_t trackIndex);
    void DrawKeyFrameMenuItems(int32_t trackIndex, int32_t keyIndex);

    // 再生モード
    PlayerComboAttackTimelineData::PlayMode GetPlayMode() const { return playMode_; }
    void SetPlayMode(PlayerComboAttackTimelineData::PlayMode mode) { playMode_ = mode; }

private:
    void DrawAddTrackPopup();
    void DrawRenditionKeyFrameEditor(int32_t trackIndex, int32_t keyIndex);

    // トラックメニューアイテム描画のヘルパー関数
    void DrawTrackMenuItem(const char* label, PlayerComboAttackTimelineData::TrackType trackType);

    // パラメータUI描画関数を登録
    void RegisterParamUIFunctions();

private:
    PlayerComboAttackData* attackData_                   = nullptr;
    KetaEngine::TimelineDrawer* timeline_                = nullptr;
    PlayerComboAttackTimelineData* data_                 = nullptr;
    PlayerComboAttackTimelineTrackBuilder* trackBuilder_ = nullptr;

    PlayerComboAttackTimelineData::PlayMode playMode_ =
        PlayerComboAttackTimelineData::PlayMode::SINGLE;
    ParamEditType selectedParamEditType_ = ParamEditType::NONE;

    // 各キーフレーム用のFileSelector
    std::unordered_map<std::string, KetaEngine::FileSelector> fileSelectorMap_;

    // パラメータUI描画関数マップ
    std::unordered_map<ParamEditType, std::function<void()>> paramUIDrawFunctions_;
};