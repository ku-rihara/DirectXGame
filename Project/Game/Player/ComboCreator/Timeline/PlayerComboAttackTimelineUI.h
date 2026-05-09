#pragma once

#include "PlayerComboAttackTimelineData.h"
#include "PlayerComboAttackTimelinePhase.h"
#include "utility/FileSelector/FileSelector.h"
#include "Editor/EffectEditorSuite/EffectEditorSuite.h"
#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>

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
        PlayerComboAttackTimelineTrackBuilder* trackBuilder,
        AttackTimelinePhase phase = AttackTimelinePhase::MAIN);

    // メインUI描画
    void DrawParamEditButtons();
    void DrawAddTrackButton();

    // コンテキストメニュー
    void DrawTrackContextMenu(int32_t trackIndex);
    void DrawKeyFrameMenuItems(int32_t trackIndex, int32_t keyIndex);

    /// <summary>
    /// エフェクトエディタースイートをセット（コンボエディターからファイル編集ウィンドウを開くために使用）
    /// </summary>
    void SetEffectEditorSuite(KetaEngine::EffectEditorSuite* suite) { effectEditorSuite_ = suite; }

private:
    void DrawAddTrackPopup();
    void DrawRenditionKeyFrameEditor(int32_t trackIndex, int32_t keyIndex);
    void DrawVibrationKeyFrameEditor(int32_t trackIndex, int32_t keyIndex);

    // トラックメニューアイテム描画のヘルパー関数
    void DrawTrackMenuItem(const char* label, PlayerComboAttackTimelineData::TrackType trackType);

    // パラメータUI描画関数を登録
    void RegisterParamUIFunctions();

private:
    // TrackType → EffectEditorType の変換（対応エディターがない場合は nullopt）
    std::optional<KetaEngine::EffectEditorType> GetEffectEditorType(
        PlayerComboAttackTimelineData::TrackType type) const;

private:
    PlayerComboAttackData* attackData_                   = nullptr;
    KetaEngine::TimelineDrawer* timeline_                = nullptr;
    PlayerComboAttackTimelineData* data_                 = nullptr;
    PlayerComboAttackTimelineTrackBuilder* trackBuilder_ = nullptr;
    KetaEngine::EffectEditorSuite* effectEditorSuite_    = nullptr;

    ParamEditType selectedParamEditType_ = ParamEditType::NONE;
    AttackTimelinePhase phase_           = AttackTimelinePhase::MAIN;

    // 各キーフレーム用のFileSelector
    std::unordered_map<std::string, KetaEngine::FileSelector> fileSelectorMap_;

    // パラメータUI描画関数マップ
    std::unordered_map<ParamEditType, std::function<void()>> paramUIDrawFunctions_;
};