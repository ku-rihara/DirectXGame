#pragma once

#include "PlayerComboAttackTimelineManager.h"
#include "utility/FileSelector/FileSelector.h"
#include <cstdint>
#include <unordered_map>

class PlayerComboAttackData;
namespace KetaEngine {
class TimeLine;
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
        KetaEngine::TimeLine* timeline,
        PlayerComboAttackTimelineManager* manager);

    // メインUI描画
    void DrawParamEditButtons();
    void DrawAddTrackButton();

    // コンテキストメニュー
    void DrawTrackContextMenu(int32_t trackIndex);
    void DrawKeyFrameMenuItems(int32_t trackIndex, int32_t keyIndex);

    // 再生モード
    PlayerComboAttackTimelineManager::PlayMode GetPlayMode() const { return playMode_; }
    void SetPlayMode(PlayerComboAttackTimelineManager::PlayMode mode) { playMode_ = mode; }

private:
    void DrawAddTrackPopup();
    void DrawRenditionKeyFrameEditor(int32_t trackIndex, int32_t keyIndex);

    // トラックメニューアイテム描画のヘルパー関数
    void DrawTrackMenuItem(const char* label, PlayerComboAttackTimelineManager::TrackType trackType);

    // パラメータUI描画関数を登録
    void RegisterParamUIFunctions();

private:
    PlayerComboAttackData* attackData_         = nullptr;
    KetaEngine::TimeLine* timeline_            = nullptr;
    PlayerComboAttackTimelineManager* manager_ = nullptr;

    PlayerComboAttackTimelineManager::PlayMode playMode_ =
        PlayerComboAttackTimelineManager::PlayMode::SINGLE;
    ParamEditType selectedParamEditType_ = ParamEditType::NONE;

    // 各キーフレーム用のFileSelector
    std::unordered_map<std::string, KetaEngine::FileSelector> fileSelectorMap_;

    // パラメータUI描画関数マップ
    std::unordered_map<ParamEditType, std::function<void()>> paramUIDrawFunctions_;
};