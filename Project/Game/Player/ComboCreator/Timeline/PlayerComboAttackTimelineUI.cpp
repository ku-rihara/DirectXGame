#include "PlayerComboAttackTimelineUI.h"
#include "../PlayerComboAttackData.h"
#include "Easing/Easing.h"
#include "utility/FileSelector/FileSelector.h"
#include <imgui.h>

void PlayerComboAttackTimelineUI::Init(PlayerComboAttackData* attackData,
    KetaEngine::TimeLine* timeline,
    PlayerComboAttackTimelineManager* manager) {
    attackData_ = attackData;
    timeline_   = timeline;
    manager_    = manager;
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

    if (ImGui::RadioButton("タイミング", selectedParamEditType_ == ParamEditType::TIMING)) {
        selectedParamEditType_ = (selectedParamEditType_ == ParamEditType::TIMING)
                                     ? ParamEditType::NONE
                                     : ParamEditType::TIMING;
    }

    // 選択されたパラメータのUIを表示
    switch (selectedParamEditType_) {
    case ParamEditType::COLLISION:
        DrawCollisionParamUI();
        break;
    case ParamEditType::MOVE:
        DrawMoveParamUI();
        break;
    case ParamEditType::TIMING:
        DrawTimingParamUI();
        break;
    case ParamEditType::NONE:
    default:
        break;
    }
}

void PlayerComboAttackTimelineUI::DrawAddTrackButton() {
    if (ImGui::Button("トラック追加")) {
        ImGui::OpenPopup("AddTrackPopup");
    }

    DrawAddTrackPopup();
}

void PlayerComboAttackTimelineUI::DrawTrackMenuItem(const char* label, PlayerComboAttackTimelineManager::TrackType trackType) {
    bool exists = manager_->IsTrackTypeAlreadyAdded(trackType);
    if (exists)
        ImGui::BeginDisabled();
    if (ImGui::MenuItem(label)) {
        manager_->AddTrack(trackType);
    }
    if (exists)
        ImGui::EndDisabled();
}

void PlayerComboAttackTimelineUI::DrawAddTrackPopup() {
    using TrackType = PlayerComboAttackTimelineManager::TrackType;

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
        auto trackType = manager_->GetTrackTypeFromIndex(trackIndex);

        ImGui::Text("トラック: %s", manager_->GetTrackTypeName(trackType));
        ImGui::Separator();

        // デフォルトトラック以外は削除可能
        bool isDefaultTrack        = false;
        const auto& defaultIndices = manager_->GetDefaultTrackIndices();
        for (size_t i = 0; i < defaultIndices.size(); ++i) {
            if (trackIndex == defaultIndices[i]) {
                isDefaultTrack = true;
                break;
            }
        }

        if (!isDefaultTrack && ImGui::MenuItem("トラックを削除")) {
            manager_->RemoveTrack(trackIndex);
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void PlayerComboAttackTimelineUI::DrawKeyFrameContextMenu(int32_t trackIndex, int32_t keyIndex) {
    using TrackType = PlayerComboAttackTimelineManager::TrackType;

    ImGui::PushID(("KeyContext_" + std::to_string(trackIndex) + "_" + std::to_string(keyIndex)).c_str());

    std::string popupId = "KeyFrameContextMenu_" + std::to_string(trackIndex) + "_" + std::to_string(keyIndex);

    if (ImGui::BeginPopup(popupId.c_str())) {
        auto trackType = manager_->GetTrackTypeFromIndex(trackIndex);

        ImGui::Text("キーフレーム編集");
        ImGui::Separator();

        // 演出系トラックの場合
        int typeInt      = static_cast<int>(trackType);
        bool isRendition = (typeInt >= static_cast<int>(TrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(TrackType::PARTICLE_EFFECT_ON_HIT));
        bool isObjAnime  = (typeInt >= static_cast<int>(TrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(TrackType::OBJ_ANIM_MAIN_HEAD));
        bool isAudio     = (typeInt >= static_cast<int>(TrackType::AUDIO_ATTACK) && typeInt <= static_cast<int>(TrackType::AUDIO_HIT));

        if (isRendition || isObjAnime || isAudio) {
            DrawRenditionKeyFrameEditor(trackIndex, keyIndex);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("キーフレームを削除")) {
            timeline_->RemoveKeyFrame(trackIndex, keyIndex);
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void PlayerComboAttackTimelineUI::DrawRenditionKeyFrameEditor(int32_t trackIndex, int32_t keyIndex) {
    auto* trackInfo = manager_->FindTrackInfo(trackIndex);

    if (!trackInfo) {
        return;
    }

    keyIndex; // 未使用

    ImGui::SeparatorText("ファイル選択");

    // ディレクトリパスを取得
    std::string directory = manager_->GetDirectoryForTrackType(trackInfo->type);

    // FileSelectorを使ってファイル選択
    static KetaEngine::FileSelector fileSelector;
    fileSelector.SelectFile("ファイル名", directory, trackInfo->fileName, "", true);

    // カメラアクションの場合のみチェックボックスを表示
    if (trackInfo->type == PlayerComboAttackTimelineManager::TrackType::CAMERA_ACTION || trackInfo->type == PlayerComboAttackTimelineManager::TrackType::CAMERA_ACTION_ON_HIT) {
        ImGui::Checkbox("Is Camera Reset", &trackInfo->isCameraReset);
    }
}

void PlayerComboAttackTimelineUI::DrawCollisionParamUI() {
    if (!attackData_) {
        return;
    }

    auto& collisionParam = attackData_->GetAttackParam().collisionParam;
    auto& attackParam    = attackData_->GetAttackParam();

    ImGui::SeparatorText("コライダーパラメータ");

    ImGui::DragFloat3("サイズ", &collisionParam.size.x, 0.01f);
    ImGui::DragFloat3("オフセット位置", &collisionParam.offsetPos.x, 0.01f);
    ImGui::DragFloat("開始時間", &collisionParam.startTime, 0.01f);
    ImGui::DragFloat("適応時間", &collisionParam.adaptTime, 0.01f);
    ImGui::InputInt("ループ回数", &collisionParam.loopNum);

    if (collisionParam.loopNum > 0) {
        ImGui::DragFloat("ループ待機時間", &collisionParam.loopWaitTime, 0.01f);
    }

    ImGui::Checkbox("プレイヤーに追従する", &collisionParam.isAlwaysFollowing);

    ImGui::Separator();
    ImGui::DragFloat("攻撃力", &attackParam.power, 0.01f);
    ImGui::DragFloat("正面のノックバック力", &attackParam.knockBackPower, 0.01f);
    ImGui::DragFloat("Y方向のノックバック力", &attackParam.blowYPower, 0.01f);
}

void PlayerComboAttackTimelineUI::DrawMoveParamUI() {
    if (!attackData_) {
        return;
    }

    auto& moveParam = attackData_->GetAttackParam().moveParam;

    ImGui::SeparatorText("移動パラメータ");

    ImGui::Checkbox("攻撃中入力による移動ができる", &moveParam.isAbleInputMoving);
    ImGui::DragFloat("開始時間", &moveParam.startTime, 0.01f);
    ImGui::DragFloat("イージングタイム", &moveParam.easeTime, 0.01f);
    ImGui::Checkbox("Yの位置を直接指定する", &moveParam.isPositionYSelect);
    ImGui::DragFloat3("移動量", &moveParam.value.x, 0.01f);
    ImGui::DragFloat("終了タイムオフセット", &moveParam.finishTimeOffset, 0.01f);

    // Easing Type
    ImGuiEasingTypeSelector("イージング", moveParam.easeType);
}

void PlayerComboAttackTimelineUI::DrawTimingParamUI() {
    if (!attackData_)
        return;

    auto& timingParam = attackData_->GetAttackParam().timingParam;

    ImGui::SeparatorText("タイミングパラメータ");

    ImGui::Checkbox("キャンセルタイムを使う", &timingParam.isCancel);
    if (timingParam.isCancel) {
        ImGui::DragFloat("キャンセル開始タイム", &timingParam.cancelTime, 0.01f, 0.0f, 10.0f);
    }

    ImGui::DragFloat("先行入力開始タイム", &timingParam.precedeInputTime, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("攻撃終了時の待機時間", &timingParam.finishWaitTime, 0.01f, 0.0f, 10.0f);
}

void PlayerComboAttackTimelineUI::DrawRenditionFileSelector(
    PlayerComboAttackTimelineManager::TrackType type,
    PlayerComboAttackTimelineManager::TrackInfo& info) {

    ImGui::SeparatorText("ファイル選択");

    // ファイル名表示
    ImGui::Text("現在のファイル: %s", info.fileName.empty() ? "なし" : info.fileName.c_str());

    // ディレクトリパスを取得
    std::string directory = manager_->GetDirectoryForTrackType(type);

    // FileSelector統合
    static KetaEngine::FileSelector fileSelector;

    char buffer[256];
    strncpy_s(buffer, info.fileName.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputText("ファイル名", buffer, sizeof(buffer))) {
        info.fileName = buffer;
    }

    // ファイルブラウザボタン
    if (ImGui::Button("参照...")) {
        ImGui::OpenPopup("FileSelectPopup");
    }

    if (ImGui::BeginPopup("FileSelectPopup")) {
        fileSelector.SelectFile("ファイル選択", directory, info.fileName, "", true);
        ImGui::EndPopup();
    }

    // キーフレーム追加位置
    ImGui::Separator();
    static int keyFramePosition = 0;
    ImGui::InputInt("キーフレーム位置", &keyFramePosition);

    if (ImGui::Button("キーフレームを追加")) {
        timeline_->AddKeyFrame(info.trackIndex, keyFramePosition, 1.0f, 1.0f);
    }
}