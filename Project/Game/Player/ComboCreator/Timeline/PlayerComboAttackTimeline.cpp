#include "PlayerComboAttackTimeline.h"
#include "../PlayerComboAttackData.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/Player.h"
#include <imgui.h>

void PlayerComboAttackTimeline::Init(PlayerComboAttackData* attackData) {
    attackData_ = attackData;

    if (!attackData_) {
        return;
    }

    timeline_.Init();

    // 各クラスの初期化
    timelineData_.Init();
    trackBuilder_.Init(attackData_, &timeline_, &timelineData_);
    parameterApplier_.Init(attackData_, &timeline_, &timelineData_);
    ui_.Init(attackData_, &timeline_, &timelineData_, &trackBuilder_);

    // トラックのセットアップ
    trackBuilder_.SetupDefaultTracks();
    trackBuilder_.SetupRenditionTracks();
    trackBuilder_.SetupObjectAnimationTracks();
    trackBuilder_.SetupAudioTracks();

    // すべてのトラックにキーフレームコールバックを設定
    SetupKeyFrameCallbacks();

    isInitialized_ = true;
}

void PlayerComboAttackTimeline::SetupKeyFrameCallbacks() {
    // キーフレームの右クリックのコールバックを設定
    for (uint32_t i = 0; i < timeline_.GetTrackCount(); ++i) {
        timeline_.SetKeyFrameRightClickCallback(i, [this](int32_t trackIdx, int32_t keyIdx) {
            ui_.DrawKeyFrameMenuItems(trackIdx, keyIdx);
        });
    }
}

void PlayerComboAttackTimeline::Update(float deltaTime) {
    if (!isInitialized_ || !attackData_) {
        return;
    }
    deltaTime;
}

void PlayerComboAttackTimeline::Draw() {
    if (!isInitialized_ || !attackData_) {
        ImGui::Text("Timeline not initialized");
        return;
    }

    ImGui::PushID("AttackTimeline");

    // 攻撃パラメータウィンドウ
    if (ImGui::Begin("攻撃パラメータ")) {
        ui_.DrawParamEditButtons();
    }
    ImGui::End();

    ImGui::Separator();

    // タイムライントラック追加
    timeline_.SetOriginalItemDrawCallBack([this]() {
        // トラック追加ボタン
        ui_.DrawAddTrackButton();
    });

    // タイムライン描画
    timeline_.Draw("攻撃タイムライン");

    // 各トラックのコンテキストメニュー処理
    for (size_t i = 0; i < timeline_.GetTrackCount(); ++i) {
        ui_.DrawTrackContextMenu(static_cast<int32_t>(i));
    }

    // タイムライン変更を毎フレームパラメータに自動適用
    ApplyToParameters();

    ImGui::PopID();
}

void PlayerComboAttackTimeline::ApplyToParameters() {
    parameterApplier_.ApplyToParameters();
}

void PlayerComboAttackTimeline::RebuildBranchTracks() {
    if (!isInitialized_) {
        return;
    }

    trackBuilder_.RebuildBranchTracks();

    // 新しいトラックにコールバックを再設定
    SetupKeyFrameCallbacks();
}

KetaEngine::TimelineDrawer* PlayerComboAttackTimeline::GetTimeline() {
    return &timeline_;
}