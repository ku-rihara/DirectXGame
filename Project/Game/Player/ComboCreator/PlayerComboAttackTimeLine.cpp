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

    // 再生モード選択
    ImGui::Text("Play Mode:");
    ImGui::SameLine();
    if (ImGui::RadioButton("Single", playMode_ == PlayMode::SINGLE)) {
        playMode_ = PlayMode::SINGLE;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Continuous", playMode_ == PlayMode::CONTINUOUS)) {
        playMode_ = PlayMode::CONTINUOUS;
    }

    // トラック追加ボタン
    DrawAddTrackButton();

    // タイムライン描画
    timeline_.Draw();

    // 各トラックのコンテキストメニュー処理
    for (size_t i = 0; i < timeline_.GetTrackCount(); ++i) {
        DrawTrackContextMenu(static_cast<int32_t>(i));
    }

    // 適用・リセットボタン
    if (ImGui::Button("Apply to Parameters")) {
        ApplyToParameters();
    }
    ImGui::SameLine();

    if (ImGui::Button("Reset from Parameters")) {
        Init(attackData_);
    }

    ImGui::PopID();

    // 連続再生モード処理
    if (playMode_ == PlayMode::CONTINUOUS && timeline_.IsPlaying() && timeline_.GetCurrentFrame() >= timeline_.GetEndFrame()) {
        AdvanceToNextAttack();
    }
}

void PlayerComboAttackTimeline::DrawAddTrackButton() {
    if (ImGui::Button("Add Track")) {
        ImGui::OpenPopup("AddTrackPopup");
    }

    DrawAddTrackPopup();
}

void PlayerComboAttackTimeline::DrawAddTrackPopup() {
    if (ImGui::BeginPopup("AddTrackPopup")) {
        ImGui::SeparatorText("Rendition (Normal)");

        if (ImGui::MenuItem("Camera Action")) {
            AddTrack(AddableTrackType::CAMERA_ACTION);
        }
        if (ImGui::MenuItem("Hit Stop")) {
            AddTrack(AddableTrackType::HIT_STOP);
        }
        if (ImGui::MenuItem("Shake Action")) {
            AddTrack(AddableTrackType::SHAKE_ACTION);
        }
        if (ImGui::MenuItem("Post Effect")) {
            AddTrack(AddableTrackType::POST_EFFECT);
        }
        if (ImGui::MenuItem("Particle Effect")) {
            AddTrack(AddableTrackType::PARTICLE_EFFECT);
        }

        ImGui::SeparatorText("Rendition (On Hit)");

        if (ImGui::MenuItem("Camera Action (On Hit)")) {
            AddTrack(AddableTrackType::CAMERA_ACTION_ON_HIT);
        }
        if (ImGui::MenuItem("Hit Stop (On Hit)")) {
            AddTrack(AddableTrackType::HIT_STOP_ON_HIT);
        }
        if (ImGui::MenuItem("Shake Action (On Hit)")) {
            AddTrack(AddableTrackType::SHAKE_ACTION_ON_HIT);
        }
        if (ImGui::MenuItem("Post Effect (On Hit)")) {
            AddTrack(AddableTrackType::POST_EFFECT_ON_HIT);
        }
        if (ImGui::MenuItem("Particle Effect (On Hit)")) {
            AddTrack(AddableTrackType::PARTICLE_EFFECT_ON_HIT);
        }

        ImGui::SeparatorText("Object Animation");

        if (ImGui::MenuItem("Head Animation")) {
            AddTrack(AddableTrackType::OBJ_ANIM_HEAD);
        }
        if (ImGui::MenuItem("Right Hand Animation")) {
            AddTrack(AddableTrackType::OBJ_ANIM_RIGHT_HAND);
        }
        if (ImGui::MenuItem("Left Hand Animation")) {
            AddTrack(AddableTrackType::OBJ_ANIM_LEFT_HAND);
        }
        if (ImGui::MenuItem("Main Head Animation")) {
            AddTrack(AddableTrackType::OBJ_ANIM_MAIN_HEAD);
        }

        ImGui::SeparatorText("Audio");

        if (ImGui::MenuItem("Attack Sound")) {
            AddTrack(AddableTrackType::AUDIO_ATTACK);
        }
        if (ImGui::MenuItem("Hit Sound")) {
            AddTrack(AddableTrackType::AUDIO_HIT);
        }

        ImGui::SeparatorText("Timing");

        if (ImGui::MenuItem("Cancel Time")) {
            AddTrack(AddableTrackType::CANCEL_TIME);
        }
        if (ImGui::MenuItem("Precede Input")) {
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

        ImGui::Text("Track: %s", GetTrackTypeName(trackType));
        ImGui::Separator();

        // デフォルトトラックのパラメータ表示
        if (trackIndex == defaultTrackIndices_[static_cast<size_t>(DefaultTrack::COLLISION)]) {
            DrawCollisionParamUI();
        } else if (trackIndex == defaultTrackIndices_[static_cast<size_t>(DefaultTrack::MOVE_EASING)]) {
            DrawMoveParamUI();
        } else if (trackIndex == defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)] || trackIndex == defaultTrackIndices_[static_cast<size_t>(DefaultTrack::PRECEDE_INPUT_START)]) {
            if (trackIndex == defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)]) {
                DrawTimingParamUI(AddableTrackType::CANCEL_TIME);
            } else {
                DrawTimingParamUI(AddableTrackType::PRECEDE_INPUT);
            }
        }
        // 追加トラックの処理
        else {
            auto it = std::find_if(addedTracks_.begin(), addedTracks_.end(),
                [trackIndex](const TrackInfo& info) {
                    return info.trackIndex == trackIndex;
                });

            if (it != addedTracks_.end()) {
                // 演出系トラック
                int typeInt      = static_cast<int>(trackType);
                bool isRendition = (typeInt >= static_cast<int>(AddableTrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(AddableTrackType::PARTICLE_EFFECT_ON_HIT));
                bool isObjAnim   = (typeInt >= static_cast<int>(AddableTrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(AddableTrackType::OBJ_ANIM_MAIN_HEAD));
                bool isAudio     = (typeInt >= static_cast<int>(AddableTrackType::AUDIO_ATTACK) && typeInt <= static_cast<int>(AddableTrackType::AUDIO_HIT));

                if (isRendition || isObjAnim || isAudio) {
                    DrawRenditionFileSelector(trackType, *it);
                } else if (trackType == AddableTrackType::CANCEL_TIME || trackType == AddableTrackType::PRECEDE_INPUT) {
                    DrawTimingParamUI(trackType);
                }
            }
        }

        ImGui::Separator();

        // デフォルトトラック以外は削除可能
        bool isDefaultTrack = false;
        for (size_t i = 0; i < defaultTrackIndices_.size(); ++i) {
            if (trackIndex == defaultTrackIndices_[i]) {
                isDefaultTrack = true;
                break;
            }
        }

        if (!isDefaultTrack && ImGui::MenuItem("Remove Track")) {
            RemoveTrack(trackIndex);
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void PlayerComboAttackTimeline::DrawCollisionParamUI() {
    auto& collisionParam = attackData_->GetAttackParam().collisionParam;
    auto& attackParam    = attackData_->GetAttackParam();

    ImGui::SeparatorText("Collision Parameters");

    if (!attackParam.isMotionOnly) {
        ImGui::DragFloat3("Size", &collisionParam.size.x, 0.01f);
        ImGui::DragFloat3("Offset Position", &collisionParam.offsetPos.x, 0.01f);
        ImGui::DragFloat("Adapt Time", &collisionParam.adaptTime, 0.01f);
        ImGui::InputInt("Loop Count", &collisionParam.loopNum);

        if (collisionParam.loopNum > 0) {
            ImGui::DragFloat("Loop Wait Time", &collisionParam.loopWaitTime, 0.01f);
        }

        ImGui::Checkbox("Always Following", &collisionParam.isAlwaysFollowing);

        ImGui::Separator();
        ImGui::DragFloat("Attack Power", &attackParam.power, 0.01f);
        ImGui::DragFloat("Knockback Power", &attackParam.knockBackPower, 0.01f);
        ImGui::DragFloat("Blow Y Power", &attackParam.blowYPower, 0.01f);
    }
}

void PlayerComboAttackTimeline::DrawMoveParamUI() {
    auto& moveParam = attackData_->GetAttackParam().moveParam;

    ImGui::SeparatorText("Move Parameters");

    ImGui::Checkbox("Enable Input Moving", &moveParam.isAbleInputMoving);
    ImGui::DragFloat("Easing Time", &moveParam.easeTime, 0.01f);
    ImGui::Checkbox("Select Y Position", &moveParam.isPositionYSelect);
    ImGui::DragFloat3("Move Value", &moveParam.value.x, 0.01f);
    ImGui::DragFloat("Finish Time Offset", &moveParam.finishTimeOffset, 0.01f);

    // Easing Type
    const char* easingTypes[] = {"Linear", "InQuad", "OutQuad", "InOutQuad", "InCubic", "OutCubic"};
    int currentEasing         = moveParam.easeType;
    if (ImGui::Combo("Easing Type", &currentEasing, easingTypes, IM_ARRAYSIZE(easingTypes))) {
        moveParam.easeType = currentEasing;
    }
}

void PlayerComboAttackTimeline::DrawTimingParamUI(AddableTrackType type) {
    auto& timingParam = attackData_->GetAttackParam().timingParam;

    ImGui::SeparatorText("Timing Parameters");

    if (type == AddableTrackType::CANCEL_TIME) {
        ImGui::Checkbox("Use Cancel", &timingParam.isCancel);
        if (timingParam.isCancel) {
            ImGui::DragFloat("Cancel Time", &timingParam.cancelTime, 0.01f, 0.0f, 10.0f);
        }
    } else if (type == AddableTrackType::PRECEDE_INPUT) {
        ImGui::DragFloat("Precede Input Time", &timingParam.precedeInputTime, 0.01f, 0.0f, 10.0f);
    }

    ImGui::DragFloat("Finish Wait Time", &timingParam.finishWaitTime, 0.01f, 0.0f, 10.0f);
}

void PlayerComboAttackTimeline::DrawRenditionFileSelector(AddableTrackType type, TrackInfo& info) {
    ImGui::SeparatorText("File Selection");

    // ファイル名表示
    ImGui::Text("Current File: %s", info.fileName.empty() ? "None" : info.fileName.c_str());

    // ディレクトリパスを取得
    std::string directory = GetDirectoryForTrackType(type);

    // FileSelector統合
    static KetaEngine::FileSelector fileSelector;

    char buffer[256];
    strncpy_s(buffer, info.fileName.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputText("File Name", buffer, sizeof(buffer))) {
        info.fileName = buffer;
    }

    // ファイルブラウザボタン
    if (ImGui::Button("Browse...")) {
        ImGui::OpenPopup("FileSelectPopup");
    }

    if (ImGui::BeginPopup("FileSelectPopup")) {
        fileSelector.SelectFile("Select File", directory, info.fileName, "", true);
        ImGui::EndPopup();
    }

    // キーフレーム追加位置
    ImGui::Separator();
    static int keyFramePosition = 0;
    ImGui::InputInt("KeyFrame Frame", &keyFramePosition);

    if (ImGui::Button("Add KeyFrame at Frame")) {
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
        int32_t trackIdx                                                   = timeline_.AddTrack("Collision");
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
        int32_t trackIdx                                                     = timeline_.AddTrack("Move Easing");
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
        int32_t trackIdx                                                      = timeline_.AddTrack("Cancel Start");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)] = trackIdx;

        int32_t cancelFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.cancelTime);
        timeline_.AddKeyFrame(trackIdx, cancelFrame, 1.0f, 1.0f);

        timeline_.SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // Precede Input Start トラック
    {
        int32_t trackIdx                                                             = timeline_.AddTrack("Precede Input Start");
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

        std::string trackName = std::string(PlayerAttackRenditionData::kRenditionTypeInfos[i].label) + " (On Hit)";
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
    // addedTracks_から削除
    auto it = std::remove_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });
    addedTracks_.erase(it, addedTracks_.end());

    // インデックスを更新（削除されたトラックより後ろのトラックのインデックスをデクリメント）
    for (auto& track : addedTracks_) {
        if (track.trackIndex > trackIndex) {
            track.trackIndex--;
        }
    }

    // タイムラインからトラック削除
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
        int32_t endFrame = timeline_.GetFirstKeyFrameFrame(moveTrackIdx);
        if (endFrame > 0) {
            // 最後のキーフレームを探す
            const auto& tracks = timeline_.GetTracks();
            if (moveTrackIdx < static_cast<int32_t>(tracks.size())) {
                const auto& keyframes = tracks[moveTrackIdx].keyframes;
                if (!keyframes.empty()) {
                    int32_t lastFrame              = keyframes.back().frame;
                    attackParam.moveParam.easeTime = KetaEngine::Frame::FrameToTime(lastFrame);
                }
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

void PlayerComboAttackTimeline::ApplyTrackToRendition(const TrackInfo& trackInfo, float timing) {
    auto& renditionData = const_cast<PlayerAttackRenditionData&>(attackData_->GetRenditionData());

    int typeInt = static_cast<int>(trackInfo.type);

    // 通常演出
    if (typeInt >= static_cast<int>(AddableTrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(AddableTrackType::PARTICLE_EFFECT)) {

        auto& param = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamFromIndex(typeInt));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;
    }
    // ヒット時演出
    else if (typeInt >= static_cast<int>(AddableTrackType::CAMERA_ACTION_ON_HIT) && typeInt <= static_cast<int>(AddableTrackType::PARTICLE_EFFECT_ON_HIT)) {

        int baseIndex = typeInt - static_cast<int>(AddableTrackType::OBJ_ANIM_HEAD);
        auto& param   = const_cast<PlayerAttackRenditionData::ObjAnimationParam&>(
            renditionData.GetObjAnimationParamFromIndex(baseIndex));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;
    }
    // オーディオ
    else if (typeInt >= static_cast<int>(AddableTrackType::AUDIO_ATTACK) && typeInt <= static_cast<int>(AddableTrackType::AUDIO_HIT)) {

        int baseIndex = typeInt - static_cast<int>(AddableTrackType::AUDIO_ATTACK);
        auto& param   = const_cast<PlayerAttackRenditionData::AudioParam&>(
            renditionData.GetAudioParamFromIndex(baseIndex));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;
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
        return "Camera Action";
    case AddableTrackType::HIT_STOP:
        return "Hit Stop";
    case AddableTrackType::SHAKE_ACTION:
        return "Shake Action";
    case AddableTrackType::POST_EFFECT:
        return "Post Effect";
    case AddableTrackType::PARTICLE_EFFECT:
        return "Particle Effect";
    case AddableTrackType::CAMERA_ACTION_ON_HIT:
        return "Camera Action (On Hit)";
    case AddableTrackType::HIT_STOP_ON_HIT:
        return "Hit Stop (On Hit)";
    case AddableTrackType::SHAKE_ACTION_ON_HIT:
        return "Shake Action (On Hit)";
    case AddableTrackType::POST_EFFECT_ON_HIT:
        return "Post Effect (On Hit)";
    case AddableTrackType::PARTICLE_EFFECT_ON_HIT:
        return "Particle Effect (On Hit)";
    case AddableTrackType::OBJ_ANIM_HEAD:
        return "Head Animation";
    case AddableTrackType::OBJ_ANIM_RIGHT_HAND:
        return "Right Hand Animation";
    case AddableTrackType::OBJ_ANIM_LEFT_HAND:
        return "Left Hand Animation";
    case AddableTrackType::OBJ_ANIM_MAIN_HEAD:
        return "Main Head Animation";
    case AddableTrackType::AUDIO_ATTACK:
        return "Attack Sound";
    case AddableTrackType::AUDIO_HIT:
        return "Hit Sound";
    case AddableTrackType::CANCEL_TIME:
        return "Cancel Time";
    case AddableTrackType::PRECEDE_INPUT:
        return "Precede Input";
    default:
        return "Unknown";
    }
}

PlayerComboAttackTimeline::AddableTrackType
PlayerComboAttackTimeline::GetTrackTypeFromIndex(int32_t trackIndex) {
    // デフォルトトラックをチェック
    for (size_t i = 0; i < defaultTrackIndices_.size(); ++i) {
        if (defaultTrackIndices_[i] == trackIndex) {
            switch (static_cast<DefaultTrack>(i)) {
            case DefaultTrack::COLLISION:
                return AddableTrackType::COUNT; // 特殊
            case DefaultTrack::MOVE_EASING:
                return AddableTrackType::COUNT; // 特殊
            case DefaultTrack::CANCEL_START:
                return AddableTrackType::CANCEL_TIME;
            case DefaultTrack::PRECEDE_INPUT_START:
                return AddableTrackType::PRECEDE_INPUT;
            default:
                break;
            }
        }
    }

    // 追加トラックから検索
    auto it = std::find_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });

    if (it != addedTracks_.end()) {
        return it->type;
    }

    return AddableTrackType::COUNT;
}
<int>(AddableTrackType::CAMERA_ACTION_ON_HIT);
auto& param = const_cast<PlayerAttackRenditionData::RenditionParam&>(
    renditionData.GetRenditionParamOnHitFromIndex(baseIndex));
param.fileName    = trackInfo.fileName;
param.startTiming = timing;
}
// オブジェクトアニメーション
else if (typeInt >= static_cast<int>(AddableTrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(AddableTrackType::OBJ_ANIM_MAIN_HEAD)) {

    int baseIndex = typeInt - static_cast