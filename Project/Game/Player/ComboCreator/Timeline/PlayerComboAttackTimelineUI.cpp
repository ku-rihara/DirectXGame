#include "PlayerComboAttackTimelineUI.h"
#include "../PlayerComboAttackData.h"
#include "PlayerComboAttackTimelineTrackBuilder.h"
#include "utility/FileSelector/FileSelector.h"
#include <imgui.h>

void PlayerComboAttackTimelineUI::Init(
    PlayerComboAttackData* attackData,
    KetaEngine::TimeLine* timeline,
    PlayerComboAttackTimelineData* data,
    PlayerComboAttackTimelineTrackBuilder* trackBuilder) {

    attackData_   = attackData;
    timeline_     = timeline;
    data_         = data;
    trackBuilder_ = trackBuilder;

    fileSelectorMap_.clear();
    RegisterParamUIFunctions();
}

void PlayerComboAttackTimelineUI::RegisterParamUIFunctions() {
    if (!attackData_)
        return;

    paramUIDrawFunctions_[ParamEditType::COLLISION] = [this]() {
        attackData_->DrawCollisionParamUI();
    };

    paramUIDrawFunctions_[ParamEditType::MOVE] = [this]() {
        attackData_->DrawMoveParamUI();
    };

    paramUIDrawFunctions_[ParamEditType::TRIGGER] = [this]() {
        attackData_->DrawTriggerParamUI();
    };

    paramUIDrawFunctions_[ParamEditType::FLAGS] = [this]() {
        attackData_->DrawFlagsParamUI();
    };

    paramUIDrawFunctions_[ParamEditType::NEXT_ATTACK] = [this]() {
        attackData_->SelectNextAttack();
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

    if (ImGui::RadioButton("攻撃発動", selectedParamEditType_ == ParamEditType::TRIGGER)) {
        selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::TRIGGER)
                                     ? ParamEditType::NONE
                                     : ParamEditType::TRIGGER;
    }
    ImGui::SameLine();

    if (ImGui::RadioButton("フラグ設定", selectedParamEditType_ == ParamEditType::FLAGS)) {
        selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::FLAGS)
                                     ? ParamEditType::NONE
                                     : ParamEditType::FLAGS;
    }
    ImGui::SameLine();

    if (ImGui::RadioButton("次の攻撃", selectedParamEditType_ == ParamEditType::NEXT_ATTACK)) {
        selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::NEXT_ATTACK)
                                     ? ParamEditType::NONE
                                     : ParamEditType::NEXT_ATTACK;
    }

    // 選択されたパラメータのUIを描画
    auto it = paramUIDrawFunctions_.find(selectedParamEditType_);
    if (it != paramUIDrawFunctions_.end()) {
        it->second();
    }

    // セーブ・ロードボタン
    ImGui::Separator();
    attackData_->DrawSaveLoadUI();
    ImGui::Separator();
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

        if (trackType == PlayerComboAttackTimelineData::TrackType::CANCEL_TIME) {
            attackData_->GetAttackParam().timingParam.isCancel = true;
        }

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

        ImGui::SeparatorText("演出 (ヒット時)");

        DrawTrackMenuItem("カメラアクション (ヒット時)", TrackType::CAMERA_ACTION_ON_HIT);
        DrawTrackMenuItem("ヒットストップ (ヒット時)", TrackType::HIT_STOP_ON_HIT);
        DrawTrackMenuItem("シェイクアクション (ヒット時)", TrackType::SHAKE_ACTION_ON_HIT);
        DrawTrackMenuItem("ポストエフェクト (ヒット時)", TrackType::POST_EFFECT_ON_HIT);
        DrawTrackMenuItem("パーティクルエフェクト (ヒット時)", TrackType::PARTICLE_EFFECT_ON_HIT);

        ImGui::SeparatorText("オブジェクトアニメーション");

        DrawTrackMenuItem("頭アニメーション", TrackType::OBJ_ANIM_HEAD);
        DrawTrackMenuItem("右手アニメーション", TrackType::OBJ_ANIM_RIGHT_HAND);
        DrawTrackMenuItem("左手アニメーション", TrackType::OBJ_ANIM_LEFT_HAND);
        DrawTrackMenuItem("メイン頭アニメーション", TrackType::OBJ_ANIM_MAIN_HEAD);

        ImGui::SeparatorText("オーディオ");

        DrawTrackMenuItem("攻撃音", TrackType::AUDIO_ATTACK);
        DrawTrackMenuItem("ヒット音", TrackType::AUDIO_HIT);

        ImGui::SeparatorText("タイミング");

        DrawTrackMenuItem("キャンセルタイム", TrackType::CANCEL_TIME);

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
    int typeInt      = static_cast<int>(trackType);
    bool isRendition = (typeInt >= static_cast<int>(TrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(TrackType::PARTICLE_EFFECT_ON_HIT));
    bool isObjAnime  = (typeInt >= static_cast<int>(TrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(TrackType::OBJ_ANIM_MAIN_HEAD));
    bool isAudio     = (typeInt >= static_cast<int>(TrackType::AUDIO_ATTACK) && typeInt <= static_cast<int>(TrackType::AUDIO_HIT));

    if (isRendition || isObjAnime || isAudio) {
        ImGui::Text("キーフレーム編集");
        ImGui::Separator();
        DrawRenditionKeyFrameEditor(trackIndex, keyIndex);
        ImGui::Separator();
    }
}

void PlayerComboAttackTimelineUI::DrawRenditionKeyFrameEditor(int32_t trackIndex, int32_t keyIndex) {
    auto* trackInfo = data_->FindTrackInfo(trackIndex);

    if (!trackInfo) {
        return;
    }

    ImGui::SeparatorText("ファイル選択");

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

    // FileSelectorを使ってファイル選択
    fileSelectorMap_[fileSelectorKey].SelectFile(
        "ファイル名",
        directory,
        trackInfo->fileName,
        "",
        true);

    // ファイル名が変更された場合、キーフレームのラベルも更新
    if (previousFileName != trackInfo->fileName) {
        timeline_->SetKeyFrameLabel(trackIndex, keyIndex, "使用ファイル:" + trackInfo->fileName);
    }

    // カメラアクションの場合のみチェックボックスを表示
    if (trackInfo->type == PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION || trackInfo->type == PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT) {
        ImGui::Checkbox("攻撃時にカメラをリセットする", &trackInfo->isCameraReset);
    }
}