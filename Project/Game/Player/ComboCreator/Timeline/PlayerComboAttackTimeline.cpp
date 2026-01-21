#include "PlayerComboAttackTimeline.h"
#include "../PlayerComboAttackData.h"
#include <imgui.h>

void PlayerComboAttackTimeline::Init(PlayerComboAttackData* attackData) {
    attackData_ = attackData;

    if (!attackData_) {
        return;
    }

    timeline_.Init();

    // 各コンポーネントの初期化
    data_.Init();
    trackBuilder_.Init(attackData_, &timeline_, &data_);
    parameterApplier_.Init(attackData_, &timeline_, &data_);
    ui_.Init(attackData_, &timeline_, &data_, &trackBuilder_);

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
    for (uint32_t i = 0; i < timeline_.GetTrackCount(); ++i) {
        timeline_.SetKeyFrameRightClickCallback(i, [this](int32_t trackIdx, int32_t keyIdx) {
            ui_.DrawKeyFrameMenuItems(trackIdx, keyIdx);
        });
    }
}

void PlayerComboAttackTimeline::Draw() {
    if (!isInitialized_ || !attackData_) {
        ImGui::Text("Timeline not initialized");
        return;
    }

    ImGui::PushID("AttackTimeline");

    // タイムライン描画前の追加UI
    timeline_.SetOriginalItemDrawCallBack([this]() {
        // 再生モード選択
        ImGui::Text("再生モード:");
        ImGui::SameLine();

        auto currentMode = ui_.GetPlayMode();
        if (ImGui::RadioButton("単体", currentMode == PlayerComboAttackTimelineData::PlayMode::SINGLE)) {
            ui_.SetPlayMode(PlayerComboAttackTimelineData::PlayMode::SINGLE);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("連続", currentMode == PlayerComboAttackTimelineData::PlayMode::CONTINUOUS)) {
            ui_.SetPlayMode(PlayerComboAttackTimelineData::PlayMode::CONTINUOUS);
        }

        ImGui::SameLine();
        ImGui::Separator();

        // パラメータ編集ボタン
        ImGui::Separator();
        ui_.DrawParamEditButtons();
        ImGui::Separator();

        ImGui::SameLine();
        ImGui::Separator();

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

    // 連続再生モード処理
    if (ui_.GetPlayMode() == PlayerComboAttackTimelineData::PlayMode::CONTINUOUS && timeline_.IsPlaying() && timeline_.GetCurrentFrame() >= timeline_.GetEndFrame()) {
        AdvanceToNextAttack();
    }
}

void PlayerComboAttackTimeline::ApplyToParameters() {
    parameterApplier_.ApplyToParameters();
}

void PlayerComboAttackTimeline::AdvanceToNextAttack() {
    if (!attackData_) {
        return;
    }

    std::string nextAttackName = attackData_->GetAttackParam().nextAttackType;

    if (nextAttackName.empty() || nextAttackName == "None") {
        timeline_.SetPlaying(false);
        timeline_.SetCurrentFrame(0);
        return;
    }
}