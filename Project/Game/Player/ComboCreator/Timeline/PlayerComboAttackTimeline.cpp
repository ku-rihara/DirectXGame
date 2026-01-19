#include "PlayerComboAttackTimeline.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"
#include <imgui.h>

void PlayerComboAttackTimeline::Init(PlayerComboAttackData* attackData) {
    attackData_ = attackData;

    if (!attackData_) {
        return;
    }

    timeline_.Init();

    // Managerの初期化
    manager_.Init(attackData_, &timeline_);

    // UIの初期化
    ui_.Init(attackData_, &timeline_, &manager_);

    // トラックのセットアップ
    manager_.SetupDefaultTracks();
    manager_.SetupRenditionTracks();
    manager_.SetupObjectAnimationTracks();
    manager_.SetupAudioTracks();

    isInitialized_ = true;
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
        if (ImGui::RadioButton("単体", currentMode == PlayerComboAttackTimelineManager::PlayMode::SINGLE)) {
            ui_.SetPlayMode(PlayerComboAttackTimelineManager::PlayMode::SINGLE);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("連続", currentMode == PlayerComboAttackTimelineManager::PlayMode::CONTINUOUS)) {
            ui_.SetPlayMode(PlayerComboAttackTimelineManager::PlayMode::CONTINUOUS);
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

    // キーフレーム右クリックメニュー処理
    for (size_t i = 0; i < timeline_.GetTrackCount(); ++i) {
        const auto& tracks = timeline_.GetTracks();
        if (i < tracks.size()) {
            for (size_t j = 0; j < tracks[i].keyframes.size(); ++j) {
                ui_.DrawKeyFrameContextMenu(static_cast<int32_t>(i), static_cast<int32_t>(j));
            }
        }
    }

    // タイムライン変更を毎フレームパラメータに自動適用
    ApplyToParameters();

    ImGui::PopID();

    // 連続再生モード処理
    if (ui_.GetPlayMode() == PlayerComboAttackTimelineManager::PlayMode::CONTINUOUS && timeline_.IsPlaying() && timeline_.GetCurrentFrame() >= timeline_.GetEndFrame()) {
        AdvanceToNextAttack();
    }
}

void PlayerComboAttackTimeline::ApplyToParameters() {
    manager_.ApplyToParameters();
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