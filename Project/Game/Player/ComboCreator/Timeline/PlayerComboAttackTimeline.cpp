#include "PlayerComboAttackTimeline.h"
#include "../PlayerComboAttackData.h"
#include "Player/ComboAttackBehavior/PreView/PlayerComboAttackPreview.h"
#include "Player/Player.h"
#include <imgui.h>

void PlayerComboAttackTimeline::Init(PlayerComboAttackData* attackData) {
    attackData_ = attackData;

    if (!attackData_) {
        return;
    }

    timeline_.Init();

    // 各クラスの初期化
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

    timeline_.SetOriginalItemDrawCallBack([this]() {
        ImGui::Text("再生モード:");
        ImGui::SameLine();
        ImGui::Separator();

        // プレビューボタン
        if (!IsPreviewPlaying()) {
            if (ImGui::Button("単体プレビュー")) {
                auto* player = attackData_->GetPlayer();
                if (player) {
                    player->ChangeComboBehavior(
                        std::make_unique<PlayerComboAttackPreview>(
                            player,
                            attackData_,
                            PlayerComboAttackPreview::PreviewMode::SINGLE,
                            &timeline_));
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("連続プレビュー")) {
                auto* player = attackData_->GetPlayer();
                if (player) {
                    player->ChangeComboBehavior(
                        std::make_unique<PlayerComboAttackPreview>(
                            player,
                            attackData_,
                            PlayerComboAttackPreview::PreviewMode::CHAIN,
                            &timeline_));
                }
            }
        } else {
            // プレビュー停止ボタン
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

            if (ImGui::Button("プレビュー停止", ImVec2(-1, 0))) {
                auto* player = attackData_->GetPlayer();
                if (player) {
                    auto* preview = dynamic_cast<PlayerComboAttackPreview*>(player->GetComboBehavior());
                    if (preview) {
                        preview->RequestStop();
                    }
                }
            }

            ImGui::PopStyleColor(3);
        }

        ImGui::SameLine();
        ImGui::Separator();

        ui_.DrawParamEditButtons();
        ImGui::Separator();

        ImGui::SameLine();
        ImGui::Separator();

        ui_.DrawAddTrackButton();
    });

    timeline_.Draw("攻撃タイムライン");

    for (size_t i = 0; i < timeline_.GetTrackCount(); ++i) {
        ui_.DrawTrackContextMenu(static_cast<int32_t>(i));
    }

    ApplyToParameters();

    ImGui::PopID();

    if (ui_.GetPlayMode() == PlayerComboAttackTimelineData::PlayMode::CONTINUOUS && timeline_.IsPlaying() && timeline_.GetCurrentFrame() >= timeline_.GetEndFrame()) {
        AdvanceToNextAttack();
    }
}

bool PlayerComboAttackTimeline::IsPreviewPlaying() const {
    if (!attackData_) {
        return false;
    }

    auto* player = attackData_->GetPlayer();
    if (!player) {
        return false;
    }

    // ComboBehaviorがPlayerComboAttackPreviewかチェック
    auto* comboBehavior = player->GetComboBehavior();
    return dynamic_cast<PlayerComboAttackPreview*>(comboBehavior) != nullptr;
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