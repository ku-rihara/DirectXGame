#pragma once

// timeline
#include "PlayerComboAttackTimelineData.h"
#include "PlayerComboAttackTimelineParameterApplier.h"
#include "PlayerComboAttackTimelinePhase.h"
#include "PlayerComboAttackTimelineTrackBuilder.h"
#include "PlayerComboAttackTimelineUI.h"
// utility
#include "Utility/TimeLine/TimelineDrawer.h"


class PlayerComboAttackData;

/// <summary>
/// プレイヤー攻撃用タイムラインエディタ
/// </summary>
class PlayerComboAttackTimeline {
public:
    PlayerComboAttackTimeline()  = default;
    ~PlayerComboAttackTimeline() = default;

    // 初期化、更新、UI描画
    void Init(PlayerComboAttackData* attackData, AttackTimelinePhase phase = AttackTimelinePhase::MAIN);
    void Update(float deltaTime);
    void Draw();

    // パラメータ適用、分岐トラック再構築
    void ApplyToParameters();
    void RebuildBranchTracks();

private:
    // キーフレーム・トラックのコールバック設定
    void SetupKeyFrameCallbacks();
    void SetupTrackCallbacks();

private:
    PlayerComboAttackData* attackData_ = nullptr;
    AttackTimelinePhase phase_         = AttackTimelinePhase::MAIN;
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

    // Setter
    void SetEffectEditorSuite(KetaEngine::EffectEditorSuite* suite);
};