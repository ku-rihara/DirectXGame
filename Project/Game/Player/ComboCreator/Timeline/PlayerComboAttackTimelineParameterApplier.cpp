#include "PlayerComboAttackTimelineParameterApplier.h"
#include "../PlayerAttackRenditionData.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"
#include <algorithm>

void PlayerComboAttackTimelineParameterApplier::Init(
    PlayerComboAttackData* attackData,
    KetaEngine::TimeLine* timeline,
    PlayerComboAttackTimelineData* data) {

    attackData_ = attackData;
    timeline_   = timeline;
    data_       = data;
}

void PlayerComboAttackTimelineParameterApplier::ApplyToParameters() {
    if (!attackData_ || !timeline_ || !data_) {
        return;
    }

    auto& attackParam = attackData_->GetAttackParam();

    // コライダー適用
    int32_t collisionTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::COLLISION);

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
    int32_t moveTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::MOVE_EASING);

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

    // 終了待機時間の適用
    int32_t finishWaitTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT);

    if (finishWaitTrackIdx >= 0) {
        const auto& tracks = timeline_->GetTracks();
        if (finishWaitTrackIdx < static_cast<int32_t>(tracks.size())) {
            const auto& keyframes = tracks[finishWaitTrackIdx].keyframes;

            if (!keyframes.empty()) {
                float waitDuration                     = keyframes[0].duration;
                attackParam.timingParam.finishWaitTime = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(waitDuration));

                // 終了待機の開始位置から移動終了位置を引いてfinishTimeOffsetを計算
                if (moveTrackIdx >= 0 && moveTrackIdx < static_cast<int32_t>(tracks.size())) {
                    const auto& moveKeyframes = tracks[moveTrackIdx].keyframes;
                    if (!moveKeyframes.empty()) {
                        int32_t waitStartFrame                 = keyframes[0].frame;
                        int32_t moveEndFrame                   = moveKeyframes[0].frame + static_cast<int32_t>(moveKeyframes[0].duration);
                        int32_t offsetFrames                   = waitStartFrame - moveEndFrame;
                        attackParam.moveParam.finishTimeOffset = KetaEngine::Frame::FrameToTime(offsetFrames);
                    }
                }
            }
        }
    }

    // キャンセル時間適用
    int32_t cancelTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::CANCEL_START);

    if (cancelTrackIdx >= 0) {
        int32_t cancelFrame                = timeline_->GetFirstKeyFrameFrame(cancelTrackIdx);
        attackParam.timingParam.cancelTime = KetaEngine::Frame::FrameToTime(cancelFrame);
    }

    // 先行入力時間適用
    int32_t precedeTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::PRECEDE_INPUT_START);

    if (precedeTrackIdx >= 0) {
        int32_t precedeFrame                     = timeline_->GetFirstKeyFrameFrame(precedeTrackIdx);
        attackParam.timingParam.precedeInputTime = KetaEngine::Frame::FrameToTime(precedeFrame);
    }

    // 演出系の適用
    for (const auto& trackInfo : data_->GetAddedTracks()) {
        int32_t frame = timeline_->GetFirstKeyFrameFrame(trackInfo.trackIndex);
        float timing  = KetaEngine::Frame::FrameToTime(frame);
        ApplyTrackToRendition(trackInfo, timing);
    }

    // 終了待機時間のキーフレーム位置を更新
    UpdateFinishWaitKeyFramePosition();

    // タイムラインの終了フレームを再計算して更新
    UpdateTimelineEndFrame();
}

void PlayerComboAttackTimelineParameterApplier::UpdateFinishWaitKeyFramePosition() {
    if (!timeline_ || !data_) {
        return;
    }

    int32_t finishWaitTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT);
    int32_t moveTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::MOVE_EASING);

    if (finishWaitTrackIdx < 0 || moveTrackIdx < 0)
        return;

    auto& tracks = timeline_->GetTracks();
    if (finishWaitTrackIdx >= static_cast<int32_t>(tracks.size()) || moveTrackIdx >= static_cast<int32_t>(tracks.size()))
        return;

    const auto& moveKeyframes = tracks[moveTrackIdx].keyframes;
    auto& finishWaitKeyframes = tracks[finishWaitTrackIdx].keyframes;

    if (moveKeyframes.empty() || finishWaitKeyframes.empty())
        return;

    // 移動終了位置を計算
    int32_t moveEndFrame = moveKeyframes[0].frame + static_cast<int32_t>(moveKeyframes[0].duration);

    // 終了待機時間のキーフレーム位置を更新
    finishWaitKeyframes[0].frame = moveEndFrame;
}

void PlayerComboAttackTimelineParameterApplier::UpdateTimelineEndFrame() {
    if (!attackData_ || !timeline_)
        return;

    int32_t totalFrames = CalculateTotalFrames();
    timeline_->SetEndFrame(totalFrames);
}

int32_t PlayerComboAttackTimelineParameterApplier::GetFinishWaitStartFrame() const {
    if (!attackData_)
        return 0;

    auto& attackParam = attackData_->GetAttackParam();

    int32_t collisionEndFrame = KetaEngine::Frame::TimeToFrame(
        attackParam.collisionParam.startTime + attackParam.collisionParam.adaptTime);

    int32_t moveEndFrame = KetaEngine::Frame::TimeToFrame(
        attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset);

    return std::max(collisionEndFrame, moveEndFrame);
}

void PlayerComboAttackTimelineParameterApplier::ApplyTrackToRendition(
    const PlayerComboAttackTimelineData::TrackInfo& trackInfo, float timing) {

    if (!attackData_) {
        return;
    }

    auto& renditionData = const_cast<PlayerAttackRenditionData&>(attackData_->GetRenditionData());
    int typeInt         = static_cast<int>(trackInfo.type);

    if (typeInt >= static_cast<int>(PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(PlayerComboAttackTimelineData::TrackType::PARTICLE_EFFECT)) {

        auto& param = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamFromIndex(typeInt));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;

    } else if (typeInt >= static_cast<int>(PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT) && typeInt <= static_cast<int>(PlayerComboAttackTimelineData::TrackType::PARTICLE_EFFECT_ON_HIT)) {

        int baseIndex = typeInt - static_cast<int>(PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT);
        auto& param   = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamOnHitFromIndex(baseIndex));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;

    } else if (typeInt >= static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_MAIN_HEAD)) {

        int baseIndex = typeInt - static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD);
        auto& param   = const_cast<PlayerAttackRenditionData::ObjAnimationParam&>(
            renditionData.GetObjAnimationParamFromIndex(baseIndex));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;

    } else if (typeInt >= static_cast<int>(PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK) && typeInt <= static_cast<int>(PlayerComboAttackTimelineData::TrackType::AUDIO_HIT)) {

        int baseIndex = typeInt - static_cast<int>(PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK);
        auto& param   = const_cast<PlayerAttackRenditionData::AudioParam&>(
            renditionData.GetAudioParamFromIndex(baseIndex));
        param.fileName    = trackInfo.fileName;
        param.startTiming = timing;
    }
}

int32_t PlayerComboAttackTimelineParameterApplier::CalculateTotalFrames() const {
    if (!attackData_) {
        return 0;
    }

    auto& attackParam = attackData_->GetAttackParam();

    float totalTime = attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset + attackParam.timingParam.finishWaitTime;

    return KetaEngine::Frame::TimeToFrame(totalTime);
}