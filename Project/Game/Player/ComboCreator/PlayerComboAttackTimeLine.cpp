#include "PlayerComboAttackTimeline.h"
#include "Frame/Frame.h"
#include "PlayerAttackRenditionData.h"
#include "PlayerComboAttackData.h"
#include "utility/FileSelector/FileSelector.h"
#include <algorithm>
#include <imgui.h>

void PlayerComboAttackTimeline::Init(PlayerComboAttackData* attackData) {
    attackData_ = attackData;

    if (!attackData_) {
        return;
    }

    timeline_.Init();

    int32_t totalFrames = CalculateTotalFrames();
    timeline_.SetEndFrame(totalFrames);

    defaultTrackIndices_.fill(-1);
    addedTracks_.clear();
    selectedParamEditType_ = -1;

    SetupDefaultTracks();
    SetupRenditionTracks();
    SetupObjectAnimationTracks();
    SetupAudioTracks();

    isInitialized_ = true;
}

void PlayerComboAttackTimeline::Draw() {
    if (!isInitialized_ || !attackData_) {
        ImGui::Text("Timeline not initialized");
        return;
    }

    ImGui::PushID("AttackTimeline");

    timeline_.SetOriginalItemDrawCallBack([this]() {
        // 再生モード選択
        ImGui::Text("再生モード:");
        ImGui::SameLine();
        if (ImGui::RadioButton("単体", playMode_ == PlayMode::SINGLE)) {
            playMode_ = PlayMode::SINGLE;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("連続", playMode_ == PlayMode::CONTINUOUS)) {
            playMode_ = PlayMode::CONTINUOUS;
        }

        ImGui::SameLine();
        ImGui::Separator();

        // パラメータ編集ボタン
        DrawParamEditButtons();

        ImGui::SameLine();
        ImGui::Separator();

        // トラック追加ボタン
        DrawAddTrackButton();
    });

    // タイムライン描画
    timeline_.Draw("攻撃タイムライン");

    // 各トラックのコンテキストメニュー処理
    for (size_t i = 0; i < timeline_.GetTrackCount(); ++i) {
        DrawTrackContextMenu(static_cast<int32_t>(i));
    }

    // キーフレーム右クリックメニュー処理
    for (size_t i = 0; i < timeline_.GetTrackCount(); ++i) {
        const auto& tracks = timeline_.GetTracks();
        if (i < tracks.size()) {
            for (size_t j = 0; j < tracks[i].keyframes.size(); ++j) {
                DrawKeyFrameContextMenu(static_cast<int32_t>(i), static_cast<int32_t>(j));
            }
        }
    }

    // 適用・リセットボタン
    if (ImGui::Button("パラメータに適用")) {
        ApplyToParameters();
    }
    ImGui::SameLine();

    if (ImGui::Button("パラメータから再読み込み")) {
        Init(attackData_);
    }

    ImGui::PopID();

    // 連続再生モード処理
    if (playMode_ == PlayMode::CONTINUOUS && timeline_.IsPlaying() && timeline_.GetCurrentFrame() >= timeline_.GetEndFrame()) {
        AdvanceToNextAttack();
    }
}

void PlayerComboAttackTimeline::DrawParamEditButtons() {
    ImGui::Text("パラメータ編集:");
    ImGui::SameLine();

    // Collisionボタン
   
    if (ImGui::Button("コライダー")) {
        selectedParamEditType_ = (selectedParamEditType_ == 0) ? -1 : 0;
    }
    if (selectedParamEditType_ == 0) {
        DrawCollisionParamUI();
    }

    ImGui::SameLine();

    // Moveボタン
    
    if (ImGui::Button("移動")) {
        selectedParamEditType_ = (selectedParamEditType_ == 1) ? -1 : 1;
    }
    if (selectedParamEditType_ == 1) {
        DrawMoveParamUI();
    }

    ImGui::SameLine();

    // Timingボタン
    
    if (ImGui::Button("タイミング")) {
        selectedParamEditType_ = (selectedParamEditType_ == 2) ? -1 : 2;
    }
    if (selectedParamEditType_ == 2) {
        DrawTimingParamUI(AddableTrackType::CANCEL_TIME);
    }
}

void PlayerComboAttackTimeline::DrawAddTrackButton() {
    if (ImGui::Button("トラック追加")) {
        ImGui::OpenPopup("AddTrackPopup");
    }

    DrawAddTrackPopup();
}

void PlayerComboAttackTimeline::DrawAddTrackPopup() {
    if (ImGui::BeginPopup("AddTrackPopup")) {
        ImGui::SeparatorText("演出 (通常)");

        if (ImGui::MenuItem("カメラアクション")) {
            AddTrack(AddableTrackType::CAMERA_ACTION);
        }
        if (ImGui::MenuItem("ヒットストップ")) {
            AddTrack(AddableTrackType::HIT_STOP);
        }
        if (ImGui::MenuItem("シェイクアクション")) {
            AddTrack(AddableTrackType::SHAKE_ACTION);
        }
        if (ImGui::MenuItem("ポストエフェクト")) {
            AddTrack(AddableTrackType::POST_EFFECT);
        }
        if (ImGui::MenuItem("パーティクルエフェクト")) {
            AddTrack(AddableTrackType::PARTICLE_EFFECT);
        }

        ImGui::SeparatorText("演出 (ヒット時)");

        if (ImGui::MenuItem("カメラアクション (ヒット時)")) {
            AddTrack(AddableTrackType::CAMERA_ACTION_ON_HIT);
        }
        if (ImGui::MenuItem("ヒットストップ (ヒット時)")) {
            AddTrack(AddableTrackType::HIT_STOP_ON_HIT);
        }
        if (ImGui::MenuItem("シェイクアクション (ヒット時)")) {
            AddTrack(AddableTrackType::SHAKE_ACTION_ON_HIT);
        }
        if (ImGui::MenuItem("ポストエフェクト (ヒット時)")) {
            AddTrack(AddableTrackType::POST_EFFECT_ON_HIT);
        }
        if (ImGui::MenuItem("パーティクルエフェクト (ヒット時)")) {
            AddTrack(AddableTrackType::PARTICLE_EFFECT_ON_HIT);
        }

        ImGui::SeparatorText("オブジェクトアニメーション");

        if (ImGui::MenuItem("頭アニメーション")) {
            AddTrack(AddableTrackType::OBJ_ANIM_HEAD);
        }
        if (ImGui::MenuItem("右手アニメーション")) {
            AddTrack(AddableTrackType::OBJ_ANIM_RIGHT_HAND);
        }
        if (ImGui::MenuItem("左手アニメーション")) {
            AddTrack(AddableTrackType::OBJ_ANIM_LEFT_HAND);
        }
        if (ImGui::MenuItem("メイン頭アニメーション")) {
            AddTrack(AddableTrackType::OBJ_ANIM_MAIN_HEAD);
        }

        ImGui::SeparatorText("オーディオ");

        if (ImGui::MenuItem("攻撃音")) {
            AddTrack(AddableTrackType::AUDIO_ATTACK);
        }
        if (ImGui::MenuItem("ヒット音")) {
            AddTrack(AddableTrackType::AUDIO_HIT);
        }

        ImGui::SeparatorText("タイミング");

        if (ImGui::MenuItem("キャンセルタイム")) {
            AddTrack(AddableTrackType::CANCEL_TIME);
        }
        if (ImGui::MenuItem("先行入力")) {
            AddTrack(AddableTrackType::PRECEDE_INPUT);
        }

        ImGui::EndPopup();
    }
}

void PlayerComboAttackTimeline::DrawTrackContextMenu(int32_t trackIndex) {
    ImGui::PushID(("TrackContext_" + std::to_string(trackIndex)).c_str());

    std::string popupId = "TrackContextMenu_" + std::to_string(trackIndex);

    if (ImGui::BeginPopup(popupId.c_str())) {
        AddableTrackType trackType = GetTrackTypeFromIndex(trackIndex);

        ImGui::Text("トラック: %s", GetTrackTypeName(trackType));
        ImGui::Separator();

        // デフォルトトラック以外は削除可能
        bool isDefaultTrack = false;
        for (size_t i = 0; i < defaultTrackIndices_.size(); ++i) {
            if (trackIndex == defaultTrackIndices_[i]) {
                isDefaultTrack = true;
                break;
            }
        }

        if (!isDefaultTrack && ImGui::MenuItem("トラックを削除")) {
            RemoveTrack(trackIndex);
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void PlayerComboAttackTimeline::DrawKeyFrameContextMenu(int32_t trackIndex, int32_t keyIndex) {
    ImGui::PushID(("KeyContext_" + std::to_string(trackIndex) + "_" + std::to_string(keyIndex)).c_str());

    std::string popupId = "KeyFrameContextMenu_" + std::to_string(trackIndex) + "_" + std::to_string(keyIndex);

    if (ImGui::BeginPopup(popupId.c_str())) {
        AddableTrackType trackType = GetTrackTypeFromIndex(trackIndex);

        ImGui::Text("キーフレーム編集");
        ImGui::Separator();

        // 演出系トラックの場合
        int typeInt      = static_cast<int>(trackType);
        bool isRendition = (typeInt >= static_cast<int>(AddableTrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(AddableTrackType::PARTICLE_EFFECT_ON_HIT));
        bool isObjAnim   = (typeInt >= static_cast<int>(AddableTrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(AddableTrackType::OBJ_ANIM_MAIN_HEAD));
        bool isAudio     = (typeInt >= static_cast<int>(AddableTrackType::AUDIO_ATTACK) && typeInt <= static_cast<int>(AddableTrackType::AUDIO_HIT));

        if (isRendition || isObjAnim || isAudio) {
            DrawRenditionKeyFrameEditor(trackIndex, keyIndex);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("キーフレームを削除")) {
            timeline_.RemoveKeyFrame(trackIndex, keyIndex);
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void PlayerComboAttackTimeline::DrawRenditionKeyFrameEditor(int32_t trackIndex, int32_t keyIndex) {
    auto it = std::find_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });

    if (it == addedTracks_.end()) {
        return;
    }
    keyIndex;
    ImGui::SeparatorText("ファイル選択");

    // ファイル名表示
    ImGui::Text("現在のファイル: %s", it->fileName.empty() ? "なし" : it->fileName.c_str());

    // ディレクトリパスを取得
    std::string directory = GetDirectoryForTrackType(it->type);

    // FileSelector統合
    static KetaEngine::FileSelector fileSelector;

    char buffer[256];
    strncpy_s(buffer, it->fileName.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputText("ファイル名", buffer, sizeof(buffer))) {
        it->fileName = buffer;
    }

    // ファイルブラウザボタン
    if (ImGui::Button("参照...")) {
        ImGui::OpenPopup("FileSelectPopup");
    }

    if (ImGui::BeginPopup("FileSelectPopup")) {
        fileSelector.SelectFile("ファイル選択", directory, it->fileName, "", true);
        ImGui::EndPopup();
    }
}

void PlayerComboAttackTimeline::DrawCollisionParamUI() {
    auto& collisionParam = attackData_->GetAttackParam().collisionParam;
    auto& attackParam    = attackData_->GetAttackParam();

    ImGui::SeparatorText("コライダーパラメータ");

    if (!attackParam.isMotionOnly) {
        ImGui::DragFloat3("サイズ", &collisionParam.size.x, 0.01f);
        ImGui::DragFloat3("オフセット位置", &collisionParam.offsetPos.x, 0.01f);
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
    } else {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "モーションのみモードが有効です");
    }
}

void PlayerComboAttackTimeline::DrawMoveParamUI() {
    auto& moveParam = attackData_->GetAttackParam().moveParam;

    ImGui::SeparatorText("移動パラメータ");

    ImGui::Checkbox("攻撃中入力による移動ができる", &moveParam.isAbleInputMoving);
    ImGui::DragFloat("イージングタイム", &moveParam.easeTime, 0.01f);
    ImGui::Checkbox("Yの位置を直接指定する", &moveParam.isPositionYSelect);
    ImGui::DragFloat3("移動量", &moveParam.value.x, 0.01f);
    ImGui::DragFloat("終了タイムオフセット", &moveParam.finishTimeOffset, 0.01f);

    // Easing Type
    const char* easingTypes[] = {"リニア", "InQuad", "OutQuad", "InOutQuad", "InCubic", "OutCubic"};
    int currentEasing         = moveParam.easeType;
    if (ImGui::Combo("イージングタイプ", &currentEasing, easingTypes, IM_ARRAYSIZE(easingTypes))) {
        moveParam.easeType = currentEasing;
    }
}

void PlayerComboAttackTimeline::DrawTimingParamUI(AddableTrackType type) {
    auto& timingParam = attackData_->GetAttackParam().timingParam;
    type;
    ImGui::SeparatorText("タイミングパラメータ");

    ImGui::Checkbox("キャンセルタイムを使う", &timingParam.isCancel);
    if (timingParam.isCancel) {
        ImGui::DragFloat("キャンセル開始タイム", &timingParam.cancelTime, 0.01f, 0.0f, 10.0f);
    }

    ImGui::DragFloat("先行入力開始タイム", &timingParam.precedeInputTime, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("攻撃終了時の待機時間", &timingParam.finishWaitTime, 0.01f, 0.0f, 10.0f);
}

void PlayerComboAttackTimeline::DrawRenditionFileSelector(AddableTrackType type, TrackInfo& info) {
    ImGui::SeparatorText("ファイル選択");

    // ファイル名表示
    ImGui::Text("現在のファイル: %s", info.fileName.empty() ? "なし" : info.fileName.c_str());

    // ディレクトリパスを取得
    std::string directory = GetDirectoryForTrackType(type);

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
        timeline_.AddKeyFrame(info.trackIndex, keyFramePosition, 1.0f, 1.0f);
    }
}

std::string PlayerComboAttackTimeline::GetDirectoryForTrackType(AddableTrackType type) const {
    const std::string basePath = "Resources/GlobalParameter/";

    switch (type) {
    case AddableTrackType::CAMERA_ACTION:
    case AddableTrackType::CAMERA_ACTION_ON_HIT:
        return basePath + "CameraAnimation/AnimationData";

    case AddableTrackType::HIT_STOP:
    case AddableTrackType::HIT_STOP_ON_HIT:
        return basePath + "TimeScale";

    case AddableTrackType::SHAKE_ACTION:
    case AddableTrackType::SHAKE_ACTION_ON_HIT:
        return basePath + "ShakeEditor";

    case AddableTrackType::POST_EFFECT:
    case AddableTrackType::POST_EFFECT_ON_HIT:
        return basePath + "PostEffect";

    case AddableTrackType::PARTICLE_EFFECT:
    case AddableTrackType::PARTICLE_EFFECT_ON_HIT:
        return basePath + "Particle/Player/Dates";

    case AddableTrackType::OBJ_ANIM_HEAD:
        return basePath + "ObjEaseAnimation/Player/Dates/";

    case AddableTrackType::OBJ_ANIM_RIGHT_HAND:
        return basePath + "ObjEaseAnimation/RightHand/Dates/";

    case AddableTrackType::OBJ_ANIM_LEFT_HAND:
        return basePath + "ObjEaseAnimation/LeftHand/Dates/";

    case AddableTrackType::OBJ_ANIM_MAIN_HEAD:
        return basePath + "ObjEaseAnimation/MainHead/Dates/";

    case AddableTrackType::AUDIO_ATTACK:
    case AddableTrackType::AUDIO_HIT:
        return "Resources/Audio/";

    default:
        return basePath;
    }
}

void PlayerComboAttackTimeline::SetupDefaultTracks() {
    auto& attackParam = attackData_->GetAttackParam();

    // Collision トラック
    {
        int32_t trackIdx                                                   = timeline_.AddTrack("コライダー");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::COLLISION)] = trackIdx;

        timeline_.AddKeyFrame(trackIdx, 0, 0.0f);
        int32_t adaptFrame = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.adaptTime);
        timeline_.AddKeyFrame(trackIdx, adaptFrame, 1.0f);

        // 右クリックコールバック設定
        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // Move Easing トラック
    {
        int32_t trackIdx                                                     = timeline_.AddTrack("移動イージング");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::MOVE_EASING)] = trackIdx;

        timeline_.AddKeyFrame(trackIdx, 0, 0.0f);
        int32_t endFrame = KetaEngine::Frame::TimeToFrame(attackParam.moveParam.easeTime);
        timeline_.AddKeyFrame(trackIdx, endFrame, 1.0f);

        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // Cancel Start トラック
    if (attackParam.timingParam.isCancel) {
        int32_t trackIdx                                                      = timeline_.AddTrack("キャンセル開始");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)] = trackIdx;

        int32_t cancelFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.cancelTime);
        timeline_.AddKeyFrame(trackIdx, cancelFrame, 1.0f, 1.0f);

        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // Precede Input Start トラック
    {
        int32_t trackIdx                                                             = timeline_.AddTrack("先行入力開始");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::PRECEDE_INPUT_START)] = trackIdx;

        int32_t precedeFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.precedeInputTime);
        timeline_.AddKeyFrame(trackIdx, precedeFrame, 1.0f, 1.0f);

        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }
}

void PlayerComboAttackTimeline::SetupRenditionTracks() {
    auto& renditionData = attackData_->GetRenditionData();

    // 通常演出
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kRenditionTypeInfos[i].label;
        int32_t trackIdx      = timeline_.AddTrack(trackName);

        TrackInfo info;
        info.type       = static_cast<AddableTrackType>(i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        addedTracks_.push_back(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_.AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);

        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // ヒット時演出
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamOnHitFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = std::string(PlayerAttackRenditionData::kRenditionTypeInfos[i].label) + " (ヒット時)";
        int32_t trackIdx      = timeline_.AddTrack(trackName);

        TrackInfo info;
        info.type       = static_cast<AddableTrackType>(static_cast<int>(AddableTrackType::CAMERA_ACTION_ON_HIT) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        addedTracks_.push_back(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_.AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);

        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }
}

void PlayerComboAttackTimeline::SetupObjectAnimationTracks() {
    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& param = renditionData.GetObjAnimationParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kObjAnimationTypeInfos[i].label;
        int32_t trackIdx      = timeline_.AddTrack(trackName);

        TrackInfo info;
        info.type = static_cast<AddableTrackType>(
            static_cast<int>(AddableTrackType::OBJ_ANIM_HEAD) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        addedTracks_.push_back(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_.AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);

        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }
}

void PlayerComboAttackTimeline::SetupAudioTracks() {
    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::AudioType::Count); ++i) {
        const auto& param = renditionData.GetAudioParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kAudioTypeInfos[i].label;
        int32_t trackIdx      = timeline_.AddTrack(trackName);

        TrackInfo info;
        info.type = static_cast<AddableTrackType>(
            static_cast<int>(AddableTrackType::AUDIO_ATTACK) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        addedTracks_.push_back(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_.AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);

        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }
}

void PlayerComboAttackTimeline::AddTrack(AddableTrackType type) {
    std::string trackName = GetTrackTypeName(type);
    int32_t trackIdx      = timeline_.AddTrack(trackName);

    TrackInfo info;
    info.type       = type;
    info.trackIndex = trackIdx;
    info.fileName   = "";
    addedTracks_.push_back(info);

    timeline_.AddKeyFrame(trackIdx, 0, 0.0f, 1.0f);

    timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
        ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
    });
}

void PlayerComboAttackTimeline::RemoveTrack(int32_t trackIndex) {
    auto it = std::remove_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });
    addedTracks_.erase(it, addedTracks_.end());

    for (auto& track : addedTracks_) {
        if (track.trackIndex > trackIndex) {
            track.trackIndex--;
        }
    }

    timeline_.RemoveTrack(trackIndex);
}

void PlayerComboAttackTimeline::ApplyToParameters() {
    if (!attackData_) {
        return;
    }

    auto& attackParam = attackData_->GetAttackParam();

    // Move Easing時間の適用
    int32_t moveTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::MOVE_EASING)];
    if (moveTrackIdx >= 0) {
        const auto& tracks = timeline_.GetTracks();
        if (moveTrackIdx < static_cast<int32_t>(tracks.size())) {
            const auto& keyframes = tracks[moveTrackIdx].keyframes;
            if (!keyframes.empty()) {
                int32_t lastFrame              = keyframes.back().frame;
                attackParam.moveParam.easeTime = KetaEngine::Frame::FrameToTime(lastFrame);
            }
        }
    }

    // Cancel Time適用
    int32_t cancelTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)];
    if (cancelTrackIdx >= 0) {
        int32_t cancelFrame                = timeline_.GetFirstKeyFrameFrame(cancelTrackIdx);
        attackParam.timingParam.cancelTime = KetaEngine::Frame::FrameToTime(cancelFrame);
    }

    // Precede Input Time適用
    int32_t precedeTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::PRECEDE_INPUT_START)];
    if (precedeTrackIdx >= 0) {
        int32_t precedeFrame                     = timeline_.GetFirstKeyFrameFrame(precedeTrackIdx);
        attackParam.timingParam.precedeInputTime = KetaEngine::Frame::FrameToTime(precedeFrame);
    }

    // 演出系の適用
    for (const auto& trackInfo : addedTracks_) {
        int32_t frame = timeline_.GetFirstKeyFrameFrame(trackInfo.trackIndex);
        float timing  = KetaEngine::Frame::FrameToTime(frame);

        ApplyTrackToRendition(trackInfo, timing);
    }
}

int32_t PlayerComboAttackTimeline::CalculateTotalFrames() const {
    if (!attackData_) {
        return 300;
    }

    auto& attackParam = attackData_->GetAttackParam();
    float totalTime   = attackParam.moveParam.easeTime + attackParam.timingParam.finishWaitTime + attackParam.moveParam.finishTimeOffset;

    return KetaEngine::Frame::TimeToFrame(totalTime);
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

const char* PlayerComboAttackTimeline::GetTrackTypeName(AddableTrackType type) const {
    switch (type) {
    case AddableTrackType::CAMERA_ACTION:
        return "カメラアクション";
    case AddableTrackType::HIT_STOP:
        return "ヒットストップ";
    case AddableTrackType::SHAKE_ACTION:
        return "シェイクアクション";
    case AddableTrackType::POST_EFFECT:
        return "ポストエフェクト";
    case AddableTrackType::PARTICLE_EFFECT:
        return "パーティクルエフェクト";
    case AddableTrackType::CAMERA_ACTION_ON_HIT:
        return "カメラアクション (ヒット時)";
    case AddableTrackType::HIT_STOP_ON_HIT:
        return "ヒットストップ (ヒット時)";
    case AddableTrackType::SHAKE_ACTION_ON_HIT:
        return "シェイクアクション (ヒット時)";
    case AddableTrackType::POST_EFFECT_ON_HIT:
        return "ポストエフェクト (ヒット時)";
    case AddableTrackType::PARTICLE_EFFECT_ON_HIT:
        return "パーティクルエフェクト (ヒット時)";
    case AddableTrackType::OBJ_ANIM_HEAD:
        return "頭アニメーション";
    case AddableTrackType::OBJ_ANIM_RIGHT_HAND:
        return "右手アニメーション";
    case AddableTrackType::OBJ_ANIM_LEFT_HAND:
        return "左手アニメーション";
    case AddableTrackType::OBJ_ANIM_MAIN_HEAD:
        return "メイン頭アニメーション";
    case AddableTrackType::AUDIO_ATTACK:
        return "攻撃音";
    case AddableTrackType::AUDIO_HIT:
        return "ヒット音";
    case AddableTrackType::CANCEL_TIME:
        return "キャンセルタイム";
    case AddableTrackType::PRECEDE_INPUT:
        return "先行入力";
    default:
        return "不明";
    }
}

PlayerComboAttackTimeline::AddableTrackType
PlayerComboAttackTimeline::GetTrackTypeFromIndex(int32_t trackIndex) {
    for (size_t i = 0; i < defaultTrackIndices_.size(); ++i) {
        if (defaultTrackIndices_[i] == trackIndex) {
            switch (static_cast<DefaultTrack>(i)) {
            case DefaultTrack::COLLISION:
                return AddableTrackType::COUNT;
            case DefaultTrack::MOVE_EASING:
                return AddableTrackType::COUNT;
            case DefaultTrack::CANCEL_START:
                return AddableTrackType::CANCEL_TIME;
            case DefaultTrack::PRECEDE_INPUT_START:
                return AddableTrackType::PRECEDE_INPUT;
            default:
                break;
            }
        }
    }

    auto it = std::find_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });

    if (it != addedTracks_.end()) {
        return it->type;
    }

    return AddableTrackType::COUNT;
}

void PlayerComboAttackTimeline::ApplyTrackToRendition(const TrackInfo& trackInfo, float timing) {
    auto& renditionData = const_cast<PlayerAttackRenditionData&>(attackData_->GetRenditionData());

    int typeInt = static_cast<int>(trackInfo.type);

    if (typeInt >= static_cast<int>(AddableTrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(AddableTrackType::PARTICLE_EFFECT)) {
        auto& param = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamFromIndex(typeInt));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;
    } else if (typeInt >= static_cast<int>(AddableTrackType::CAMERA_ACTION_ON_HIT) && typeInt <= static_cast<int>(AddableTrackType::PARTICLE_EFFECT_ON_HIT)) {
        int baseIndex = typeInt - static_cast<int>(AddableTrackType::CAMERA_ACTION_ON_HIT);
        auto& param   = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamOnHitFromIndex(baseIndex));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;
    } else if (typeInt >= static_cast<int>(AddableTrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(AddableTrackType::OBJ_ANIM_MAIN_HEAD)) {
        int baseIndex = typeInt - static_cast<int>(AddableTrackType::OBJ_ANIM_HEAD);
        auto& param   = const_cast<PlayerAttackRenditionData::ObjAnimationParam&>(
            renditionData.GetObjAnimationParamFromIndex(baseIndex));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;
    } else if (typeInt >= static_cast<int>(AddableTrackType::AUDIO_ATTACK) && typeInt <= static_cast<int>(AddableTrackType::AUDIO_HIT)) {
        int baseIndex = typeInt - static_cast<int>(AddableTrackType::AUDIO_ATTACK);
        auto& param   = const_cast<PlayerAttackRenditionData::AudioParam&>(
            renditionData.GetAudioParamFromIndex(baseIndex));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;
    }
}