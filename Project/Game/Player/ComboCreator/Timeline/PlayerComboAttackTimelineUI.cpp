#include "PlayerComboAttackTimelineUI.h"
#include "../PlayerComboAttackData.h"
#include "../PlayerComboAttackController.h"
#include "PlayerComboAttackTimelineTrackBuilder.h"
#include "utility/FileSelector/FileSelector.h"
#include "Player/Player.h"
#include <imgui.h>
#include <optional>

void PlayerComboAttackTimelineUI::Init(
    PlayerComboAttackData* attackData,
    KetaEngine::TimelineDrawer* timeline,
    PlayerComboAttackTimelineData* data,
    PlayerComboAttackTimelineTrackBuilder* trackBuilder,
    AttackTimelinePhase phase) {

    // クラスセット
    attackData_   = attackData;
    timeline_     = timeline;
    data_         = data;
    trackBuilder_ = trackBuilder;
    phase_        = phase;

    fileSelectorMap_.clear();
    RegisterParamUIFunctions();
}

void PlayerComboAttackTimelineUI::RegisterParamUIFunctions() {
    if (!attackData_) {
        return;
    }

    paramUIDrawFunctions_[ParamEditType::COLLISION] = [this]() {
        attackData_->DrawCollisionParamUIForPhase(phase_);
    };

    paramUIDrawFunctions_[ParamEditType::MOVE] = [this]() {
        attackData_->DrawMoveParamUIForPhase(phase_);
    };

    // 攻撃発動パラメータはMAINフェーズのみ
    if (phase_ == AttackTimelinePhase::MAIN) {
        paramUIDrawFunctions_[ParamEditType::TRIGGER] = [this]() {
            bool isFirstAttack = true;
            if (attackData_->GetPlayer()) {
                auto* controller = attackData_->GetPlayer()->GetComboAttackController();
                if (controller) {
                    isFirstAttack = controller->IsFirstAttack(attackData_->GetGroupName());
                }
            }
            attackData_->DrawTriggerParamUI(isFirstAttack);
        };
    }

    paramUIDrawFunctions_[ParamEditType::FLAGS] = [this]() {
        attackData_->DrawFlagsParamUIForPhase(phase_);
    };

    paramUIDrawFunctions_[ParamEditType::NEXT_ATTACK] = [this]() {
        attackData_->DrawComboBranchesUI();
    };
}

void PlayerComboAttackTimelineUI::DrawParamEditButtons() {
    ImGui::Text("パラメータ編集:");
    ImGui::SameLine();

    if (ImGui::RadioButton("コライダー", selectedParamEditType_ == ParamEditType::COLLISION)) {
        selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::COLLISION)
                                     ? ParamEditType::NONE
                                     : ParamEditType::COLLISION;
    }
    ImGui::SameLine();

    if (ImGui::RadioButton("移動", selectedParamEditType_ == ParamEditType::MOVE)) {
        selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::MOVE)
                                     ? ParamEditType::NONE
                                     : ParamEditType::MOVE;
    }
    ImGui::SameLine();

    if (phase_ == AttackTimelinePhase::MAIN) {
        if (ImGui::RadioButton("攻撃発動", selectedParamEditType_ == ParamEditType::TRIGGER)) {
            selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::TRIGGER)
                                         ? ParamEditType::NONE
                                         : ParamEditType::TRIGGER;
        }
        ImGui::SameLine();
    }

    if (ImGui::RadioButton("フラグ設定", selectedParamEditType_ == ParamEditType::FLAGS)) {
        selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::FLAGS)
                                     ? ParamEditType::NONE
                                     : ParamEditType::FLAGS;
    }
    ImGui::SameLine();

    if (ImGui::RadioButton("コンボ分岐", selectedParamEditType_ == ParamEditType::NEXT_ATTACK)) {
        selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::NEXT_ATTACK)
                                     ? ParamEditType::NONE
                                     : ParamEditType::NEXT_ATTACK;
    }

    // 選択されたパラメータのUIを描画
    auto it = paramUIDrawFunctions_.find(selectedParamEditType_);
    if (it != paramUIDrawFunctions_.end()) {
        it->second();
    }
}

void PlayerComboAttackTimelineUI::DrawAddTrackButton() {
    if (ImGui::Button("トラック追加")) {
        ImGui::OpenPopup("AddTrackPopup");
    }

    DrawAddTrackPopup();
}

void PlayerComboAttackTimelineUI::DrawTrackMenuItem(
    const char* label,
    PlayerComboAttackTimelineData::TrackType trackType) {

    bool exists = data_->IsTrackTypeAlreadyAdded(trackType);
    if (exists) {
        ImGui::BeginDisabled();
    }

    if (ImGui::MenuItem(label)) {
        trackBuilder_->AddTrack(trackType);

        // 新しく追加されたトラックにコールバックを設定
        uint32_t newTrackIndex = static_cast<uint32_t>(timeline_->GetTrackCount() - 1);
        timeline_->SetKeyFrameRightClickCallback(newTrackIndex,
            [this](int32_t trackIdx, int32_t keyIdx) {
                DrawKeyFrameMenuItems(trackIdx, keyIdx);
            });
    }

    if (exists) {
        ImGui::EndDisabled();
    }
}

void PlayerComboAttackTimelineUI::DrawAddTrackPopup() {
    using TrackType = PlayerComboAttackTimelineData::TrackType;

    if (ImGui::BeginPopup("AddTrackPopup")) {
        ImGui::SeparatorText("演出 (通常)");

        DrawTrackMenuItem("カメラアクション", TrackType::CAMERA_ACTION);
        DrawTrackMenuItem("ヒットストップ", TrackType::HIT_STOP);
        DrawTrackMenuItem("シェイクアクション", TrackType::SHAKE_ACTION);
        DrawTrackMenuItem("ポストエフェクト", TrackType::POST_EFFECT);
        DrawTrackMenuItem("パーティクルエフェクト", TrackType::PARTICLE_EFFECT);
        DrawTrackMenuItem("攻撃音", TrackType::AUDIO_ATTACK);

        ImGui::SeparatorText("演出 (ヒット時)");

        DrawTrackMenuItem("カメラアクション (ヒット時)", TrackType::CAMERA_ACTION_ON_HIT);
        DrawTrackMenuItem("ヒットストップ (ヒット時)", TrackType::HIT_STOP_ON_HIT);
        DrawTrackMenuItem("シェイクアクション (ヒット時)", TrackType::SHAKE_ACTION_ON_HIT);
        DrawTrackMenuItem("ポストエフェクト (ヒット時)", TrackType::POST_EFFECT_ON_HIT);
        DrawTrackMenuItem("パーティクルエフェクト (ヒット時)", TrackType::PARTICLE_EFFECT_ON_HIT);
        DrawTrackMenuItem("攻撃ヒット音", TrackType::AUDIO_ATTACK_ON_HIT);

        ImGui::SeparatorText("オブジェクトアニメーション");

        DrawTrackMenuItem("頭アニメーション", TrackType::OBJ_ANIM_HEAD);
        DrawTrackMenuItem("右手アニメーション", TrackType::OBJ_ANIM_RIGHT_HAND);
        DrawTrackMenuItem("左手アニメーション", TrackType::OBJ_ANIM_LEFT_HAND);
        DrawTrackMenuItem("メイン頭アニメーション", TrackType::OBJ_ANIM_MAIN_HEAD);

        ImGui::SeparatorText("トレイル");

        DrawTrackMenuItem("MainHead追従トレイル", TrackType::RIBBON_TRAIL_MAIN_HEAD);

        ImGui::SeparatorText("その他");

        DrawTrackMenuItem("コントローラ振動", TrackType::VIBRATION);

        ImGui::EndPopup();
    }
}

void PlayerComboAttackTimelineUI::DrawTrackContextMenu(int32_t trackIndex) {
    ImGui::PushID(("TrackContext_" + std::to_string(trackIndex)).c_str());

    std::string popupId = "TrackContextMenu_" + std::to_string(trackIndex);

    if (ImGui::BeginPopup(popupId.c_str())) {
        auto trackType = data_->GetTrackTypeFromIndex(trackIndex);

        ImGui::Text("トラック: %s", data_->GetTrackTypeName(trackType));
        ImGui::Separator();

        // デフォルトトラック以外は削除可能
        bool isDefaultTrack        = false;
        const auto& defaultIndices = data_->GetDefaultTrackIndices();
        for (size_t i = 0; i < defaultIndices.size(); ++i) {
            if (trackIndex == defaultIndices[i]) {
                isDefaultTrack = true;
                break;
            }
        }

        if (!isDefaultTrack && ImGui::MenuItem("トラックを削除")) {
            trackBuilder_->RemoveTrack(trackIndex);
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void PlayerComboAttackTimelineUI::DrawKeyFrameMenuItems(int32_t trackIndex, int32_t keyIndex) {
    using TrackType = PlayerComboAttackTimelineData::TrackType;

    auto trackType = data_->GetTrackTypeFromIndex(trackIndex);

    // 演出系トラックの場合
    int typeInt          = static_cast<int>(trackType);
    bool isRendition     = (typeInt >= static_cast<int>(TrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(TrackType::AUDIO_ATTACK_ON_HIT));
    bool isObjAnime      = (typeInt >= static_cast<int>(TrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(TrackType::OBJ_ANIM_MAIN_HEAD));
    bool isTrailMainHead = (trackType == TrackType::RIBBON_TRAIL_MAIN_HEAD);

    bool isVibration = (trackType == TrackType::VIBRATION);

    if (isRendition || isObjAnime || isTrailMainHead) {
        ImGui::Text("キーフレーム編集");
        ImGui::Separator();
        DrawRenditionKeyFrameEditor(trackIndex, keyIndex);
        ImGui::Separator();
    } else if (isVibration) {
        ImGui::Text("キーフレーム編集");
        ImGui::Separator();
        DrawVibrationKeyFrameEditor(trackIndex, keyIndex);
        ImGui::Separator();
    }
}

std::optional<KetaEngine::EffectEditorType> PlayerComboAttackTimelineUI::GetEffectEditorType(
    PlayerComboAttackTimelineData::TrackType type) const {

    using TrackType      = PlayerComboAttackTimelineData::TrackType;
    using EffectEditorType = KetaEngine::EffectEditorType;

    switch (type) {
    case TrackType::CAMERA_ACTION:
    case TrackType::CAMERA_ACTION_ON_HIT:
        return EffectEditorType::Camera;
    case TrackType::HIT_STOP:
    case TrackType::HIT_STOP_ON_HIT:
        return EffectEditorType::TimeScale;
    case TrackType::SHAKE_ACTION:
    case TrackType::SHAKE_ACTION_ON_HIT:
        return EffectEditorType::Shake;
    case TrackType::PARTICLE_EFFECT:
    case TrackType::PARTICLE_EFFECT_ON_HIT:
        return EffectEditorType::Particle;
    case TrackType::OBJ_ANIM_HEAD:
    case TrackType::OBJ_ANIM_RIGHT_HAND:
    case TrackType::OBJ_ANIM_LEFT_HAND:
    case TrackType::OBJ_ANIM_MAIN_HEAD:
        return EffectEditorType::ObjEaseAnimation;
    case TrackType::RIBBON_TRAIL_MAIN_HEAD:
        return EffectEditorType::RibbonTrail;
    default:
        return std::nullopt; 
    }
}

void PlayerComboAttackTimelineUI::DrawRenditionKeyFrameEditor(int32_t trackIndex, int32_t keyIndex) {
    auto* trackInfo = data_->FindTrackInfo(trackIndex);

    if (!trackInfo) {
        return;
    }

    bool isTrailOnlyTrack = (trackInfo->type == PlayerComboAttackTimelineData::TrackType::RIBBON_TRAIL_MAIN_HEAD);

    ImGui::SeparatorText(isTrailOnlyTrack ? "トレイルファイル選択" : "ファイル選択");

    // ディレクトリパスを取得
    std::string directory = data_->GetDirectoryForTrackType(trackInfo->type);

    // FileSelectorのキー
    std::string fileSelectorKey = std::to_string(trackIndex) + "_" + std::to_string(keyIndex);

    // FileSelectorが存在しない場合は作成
    if (fileSelectorMap_.find(fileSelectorKey) == fileSelectorMap_.end()) {
        fileSelectorMap_[fileSelectorKey] = KetaEngine::FileSelector();
    }

    // 現在のファイル名を保持
    std::string previousFileName = trackInfo->fileName;

    const char* fileLabel = isTrailOnlyTrack ? "トレイルファイル" : "ファイル名";

    // FileSelectorを使ってファイル選択
    fileSelectorMap_[fileSelectorKey].SelectFile(
        fileLabel,
        directory,
        trackInfo->fileName,
        "",
        true);

    // ファイル名が変更された場合、キーフレームのラベルも更新
    if (previousFileName != trackInfo->fileName) {
        std::string newLabel = isTrailOnlyTrack
            ? ("トレイル:" + trackInfo->fileName)
            : ("使用ファイル:" + trackInfo->fileName);
        timeline_->SetKeyFrameLabel(trackIndex, keyIndex, newLabel);
    }

    // 右手・左手アニメーションの場合のみトレイルファイル選択を表示
    if (trackInfo->type == PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_RIGHT_HAND ||
        trackInfo->type == PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_LEFT_HAND) {

        ImGui::SeparatorText("トレイル選択");

        std::string trailSelectorKey = fileSelectorKey + "_trail";
        if (fileSelectorMap_.find(trailSelectorKey) == fileSelectorMap_.end()) {
            fileSelectorMap_[trailSelectorKey] = KetaEngine::FileSelector();
        }

        fileSelectorMap_[trailSelectorKey].SelectFile(
            "トレイルファイル",
            "Resources/GlobalParameter/RibbonTrail/Player/Dates",
            trackInfo->trailFileName,
            "",
            true);
    }

    // MainHead追従トレイル
    if (trackInfo->type == PlayerComboAttackTimelineData::TrackType::RIBBON_TRAIL_MAIN_HEAD) {
        return;
    }

    // カメラアクションの場合のみチェックボックスを表示
    if (trackInfo->type == PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION || trackInfo->type == PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT) {
        ImGui::Checkbox("攻撃時にカメラをリセットする", &trackInfo->isCameraReset);
    }

    // オーディオの場合のみボリュームスライダーを表示
    if (trackInfo->type == PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK  || trackInfo->type == PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK_ON_HIT ) {
        ImGui::SliderFloat("ボリューム", &trackInfo->volume, 0.0f, 1.0f);
    }

    // OnHitオーディオの場合のみrepeatOnDamageを表示
    if (trackInfo->type == PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK_ON_HIT) {
        ImGui::Checkbox("ダメージごとに再生", &trackInfo->repeatOnDamage);
    }

    // 選択ファイルをインラインエディターで編集するボタン
    if (effectEditorSuite_ && !trackInfo->fileName.empty()) {
        auto editorType = GetEffectEditorType(trackInfo->type);
        if (editorType.has_value()) {
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.3f, 0.7f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.45f, 0.85f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.2f, 0.6f, 1.0f));
            if (ImGui::Button(("選択ファイルを編集##" + fileSelectorKey).c_str())) {
                std::string category = data_->GetCategoryForTrackType(trackInfo->type);
                effectEditorSuite_->OpenInlineEditor(editorType.value(), trackInfo->fileName, category);
            }
            ImGui::PopStyleColor(3);
        }
    }
}

void PlayerComboAttackTimelineUI::DrawVibrationKeyFrameEditor(int32_t trackIndex, int32_t keyIndex) {
    auto* trackInfo = data_->FindTrackInfo(trackIndex);
    if (!trackInfo) {
        return;
    }

    ImGui::SeparatorText("振動パラメータ");

    // 振動強度スライダー
    float prevIntensity = trackInfo->vibrationIntensity;
    if (ImGui::SliderFloat("振動強度", &trackInfo->vibrationIntensity, 0.0f, 1.0f)) {
        if (prevIntensity != trackInfo->vibrationIntensity) {
            std::string newLabel = "振動強度:" + std::to_string(trackInfo->vibrationIntensity);
            timeline_->SetKeyFrameLabel(trackIndex, keyIndex, newLabel);
        }
    }

    ImGui::Checkbox("ヒット時のみ振動", &trackInfo->triggerByHit);
    ImGui::Checkbox("ダメージごとに振動", &trackInfo->repeatOnDamage);
}