#include "PlayerComboAttackTimelineManager.h"
#include "../PlayerAttackRenditionData.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"
#include <algorithm>
#include <imgui.h>

void PlayerComboAttackTimelineManager::Init(PlayerComboAttackData* attackData, KetaEngine::TimeLine* timeline) {
    attackData_ = attackData;
    timeline_   = timeline;

    if (!attackData_ || !timeline_) {
        return;
    }

    defaultTrackIndices_.fill(-1);
    addedTracks_.clear();
}

void PlayerComboAttackTimelineManager::SetupDefaultTracks() {
    if (!attackData_ || !timeline_)
        return;

    auto& attackParam = attackData_->GetAttackParam();

    // 総フレーム数を計算して設定
    int32_t totalFrames = CalculateTotalFrames();
    timeline_->SetEndFrame(totalFrames);

    // コライダートラック（キーフレーム1つ、開始時間から適応時間分の長さ）
    {
        int32_t trackIdx                                                   = timeline_->AddTrack("コライダー");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::COLLISION)] = trackIdx;

        int32_t startFrame     = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.startTime);
        int32_t durationFrames = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.adaptTime);

        timeline_->AddKeyFrame(trackIdx, startFrame, 1.0f, static_cast<float>(durationFrames));

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // 移動イージングトラック（キーフレーム1つ、開始時間からイージング時間分の長さ）
    {
        int32_t trackIdx                                                     = timeline_->AddTrack("移動イージング");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::MOVE_EASING)] = trackIdx;

        int32_t startFrame     = KetaEngine::Frame::TimeToFrame(attackParam.moveParam.startTime);
        int32_t durationFrames = KetaEngine::Frame::TimeToFrame(attackParam.moveParam.easeTime);

        timeline_->AddKeyFrame(trackIdx, startFrame, 1.0f, static_cast<float>(durationFrames));

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // キャンセル開始トラック
    if (attackParam.timingParam.isCancel) {
        int32_t trackIdx                                                      = timeline_->AddTrack("キャンセル開始");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)] = trackIdx;

        int32_t cancelFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.cancelTime);
        timeline_->AddKeyFrame(trackIdx, cancelFrame, 1.0f, static_cast<float>(totalFrames - cancelFrame));

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // 先行入力開始トラック
    {
        int32_t trackIdx                                                             = timeline_->AddTrack("先行入力開始");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::PRECEDE_INPUT_START)] = trackIdx;

        int32_t precedeFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.precedeInputTime);
        timeline_->AddKeyFrame(trackIdx, precedeFrame, 1.0f, static_cast<float>(totalFrames - precedeFrame));

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }

    // 攻撃終了待機トラック
    {
        int32_t trackIdx                                                     = timeline_->AddTrack("攻撃終了待機");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::FINISH_WAIT)] = trackIdx;

        float waitStartTime    = attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset;
        int32_t waitStartFrame = KetaEngine::Frame::TimeToFrame(waitStartTime);
        int32_t waitDuration   = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.finishWaitTime);

        timeline_->AddKeyFrame(trackIdx, waitStartFrame, 1.0f, static_cast<float>(waitDuration));

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }
}

void PlayerComboAttackTimelineManager::SetupRenditionTracks() {
    if (!attackData_ || !timeline_)
        return;

    auto& renditionData = attackData_->GetRenditionData();

    // 通常演出
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kRenditionTypeInfos[i].label;
        int32_t trackIdx      = timeline_->AddTrack(trackName);

        TrackInfo info;
        info.type       = static_cast<AddableTrackType>(i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        addedTracks_.push_back(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
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
        int32_t trackIdx      = timeline_->AddTrack(trackName);

        TrackInfo info;
        info.type       = static_cast<AddableTrackType>(static_cast<int>(AddableTrackType::CAMERA_ACTION_ON_HIT) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        addedTracks_.push_back(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }
}

void PlayerComboAttackTimelineManager::SetupObjectAnimationTracks() {
    if (!attackData_ || !timeline_)
        return;

    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& param = renditionData.GetObjAnimationParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kObjAnimationTypeInfos[i].label;
        int32_t trackIdx      = timeline_->AddTrack(trackName);

        TrackInfo info;
        info.type = static_cast<AddableTrackType>(
            static_cast<int>(AddableTrackType::OBJ_ANIM_HEAD) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        addedTracks_.push_back(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }
}

void PlayerComboAttackTimelineManager::SetupAudioTracks() {
    if (!attackData_ || !timeline_)
        return;

    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::AudioType::Count); ++i) {
        const auto& param = renditionData.GetAudioParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kAudioTypeInfos[i].label;
        int32_t trackIdx      = timeline_->AddTrack(trackName);

        TrackInfo info;
        info.type = static_cast<AddableTrackType>(
            static_cast<int>(AddableTrackType::AUDIO_ATTACK) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        addedTracks_.push_back(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);

        timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
            ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
        });
    }
}

bool PlayerComboAttackTimelineManager::IsTrackTypeAlreadyAdded(AddableTrackType type) const {
    // デフォルトトラックのチェック
    switch (type) {
    case AddableTrackType::CANCEL_TIME:
        return defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)] >= 0;
    case AddableTrackType::PRECEDE_INPUT:
        return defaultTrackIndices_[static_cast<size_t>(DefaultTrack::PRECEDE_INPUT_START)] >= 0;
    case AddableTrackType::FINISH_WAIT_TIME:
        return defaultTrackIndices_[static_cast<size_t>(DefaultTrack::FINISH_WAIT)] >= 0;
    default:
        break;
    }

    // 追加されたトラックのチェック
    for (const auto& track : addedTracks_) {
        if (track.type == type) {
            return true;
        }
    }

    return false;
}

void PlayerComboAttackTimelineManager::AddTrack(AddableTrackType type) {
    if (!timeline_)
        return;

    // 既に追加されているかチェック
    if (IsTrackTypeAlreadyAdded(type)) {
        return;
    }

    std::string trackName = GetTrackTypeName(type);
    int32_t trackIdx      = timeline_->AddTrack(trackName);

    TrackInfo info;
    info.type       = type;
    info.trackIndex = trackIdx;
    info.fileName   = "";
    addedTracks_.push_back(info);

    timeline_->AddKeyFrame(trackIdx, 0, 0.0f, 1.0f);

    timeline_->SetTrackRightClickCallback(trackIdx, [this, trackIdx](int32_t) {
        ImGui::OpenPopup(("TrackContextMenu_" + std::to_string(trackIdx)).c_str());
    });
}

void PlayerComboAttackTimelineManager::RemoveTrack(int32_t trackIndex) {
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

    if (timeline_) {
        timeline_->RemoveTrack(trackIndex);
    }
}

void PlayerComboAttackTimelineManager::ApplyToParameters() {
    if (!attackData_ || !timeline_)
        return;

    auto& attackParam = attackData_->GetAttackParam();

    // コライダー適用
    int32_t collisionTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::COLLISION)];
    if (collisionTrackIdx >= 0) {
        const auto& tracks = timeline_->GetTracks();
        if (collisionTrackIdx < static_cast<int32_t>(tracks.size())) {
            const auto& keyframes = tracks[collisionTrackIdx].keyframes;
            if (!keyframes.empty()) {
                int32_t startFrame = keyframes[0].frame;
                float duration     = keyframes[0].duration;

                attackParam.collisionParam.startTime = KetaEngine::Frame::FrameToTime(startFrame);
                attackParam.collisionParam.adaptTime = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(duration));
            }
        }
    }

    // 移動イージング時間の適用
    int32_t moveTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::MOVE_EASING)];
    if (moveTrackIdx >= 0) {
        const auto& tracks = timeline_->GetTracks();
        if (moveTrackIdx < static_cast<int32_t>(tracks.size())) {
            const auto& keyframes = tracks[moveTrackIdx].keyframes;
            if (!keyframes.empty()) {
                int32_t startFrame = keyframes[0].frame;
                float duration     = keyframes[0].duration;

                attackParam.moveParam.startTime = KetaEngine::Frame::FrameToTime(startFrame);
                attackParam.moveParam.easeTime  = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(duration));
            }
        }
    }

    // キャンセル時間適用
    int32_t cancelTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)];
    if (cancelTrackIdx >= 0) {
        int32_t cancelFrame                = timeline_->GetFirstKeyFrameFrame(cancelTrackIdx);
        attackParam.timingParam.cancelTime = KetaEngine::Frame::FrameToTime(cancelFrame);
    }

    // 先行入力時間適用
    int32_t precedeTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::PRECEDE_INPUT_START)];
    if (precedeTrackIdx >= 0) {
        int32_t precedeFrame                     = timeline_->GetFirstKeyFrameFrame(precedeTrackIdx);
        attackParam.timingParam.precedeInputTime = KetaEngine::Frame::FrameToTime(precedeFrame);
    }

    // 攻撃終了待機時間適用
    int32_t finishWaitTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::FINISH_WAIT)];
    if (finishWaitTrackIdx >= 0) {
        const auto& tracks = timeline_->GetTracks();
        if (finishWaitTrackIdx < static_cast<int32_t>(tracks.size())) {
            const auto& keyframes = tracks[finishWaitTrackIdx].keyframes;
            if (!keyframes.empty()) {
                float waitDuration                     = keyframes[0].duration;
                attackParam.timingParam.finishWaitTime = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(waitDuration));
            }
        }
    }

    // 演出系の適用
    for (const auto& trackInfo : addedTracks_) {
        int32_t frame = timeline_->GetFirstKeyFrameFrame(trackInfo.trackIndex);
        float timing  = KetaEngine::Frame::FrameToTime(frame);
        ApplyTrackToRendition(trackInfo, timing);
    }
}

void PlayerComboAttackTimelineManager::ApplyTrackToRendition(const TrackInfo& trackInfo, float timing) {
    if (!attackData_) {
        return;
    }

    auto& renditionData = const_cast<PlayerAttackRenditionData&>(attackData_->GetRenditionData());
    int typeInt         = static_cast<int>(trackInfo.type);

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

const char* PlayerComboAttackTimelineManager::GetTrackTypeName(AddableTrackType type) const {
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
    case AddableTrackType::FINISH_WAIT_TIME:
        return "攻撃終了待機";
    default:
        return "不明";
    }
}

PlayerComboAttackTimelineManager::AddableTrackType
PlayerComboAttackTimelineManager::GetTrackTypeFromIndex(int32_t trackIndex) const {
    for (size_t i = 0; i < defaultTrackIndices_.size(); ++i) {
        if (defaultTrackIndices_[i] == trackIndex) {
            switch (static_cast<DefaultTrack>(i)) {
            case DefaultTrack::CANCEL_START:
                return AddableTrackType::CANCEL_TIME;
            case DefaultTrack::PRECEDE_INPUT_START:
                return AddableTrackType::PRECEDE_INPUT;
            case DefaultTrack::FINISH_WAIT:
                return AddableTrackType::FINISH_WAIT_TIME;
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

std::string PlayerComboAttackTimelineManager::GetDirectoryForTrackType(AddableTrackType type) const {
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

int32_t PlayerComboAttackTimelineManager::CalculateTotalFrames() const {
    if (!attackData_) {
        return 300;
    }

    auto& attackParam = attackData_->GetAttackParam();

    // 攻撃終了待機時間が終わるまでを計算
    float totalTime = attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset + attackParam.timingParam.finishWaitTime;

    return KetaEngine::Frame::TimeToFrame(totalTime);
}

PlayerComboAttackTimelineManager::TrackInfo*
PlayerComboAttackTimelineManager::FindTrackInfo(int32_t trackIndex) {
    auto it = std::find_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });

    if (it != addedTracks_.end()) {
        return &(*it);
    }
    return nullptr;
}