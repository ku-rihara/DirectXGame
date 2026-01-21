#include "PlayerComboAttackTimelineTrackBuilder.h"
#include "../PlayerAttackRenditionData.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"

void PlayerComboAttackTimelineTrackBuilder::Init(
    PlayerComboAttackData* attackData,
    KetaEngine::TimeLine* timeline,
    PlayerComboAttackTimelineData* data) {

    attackData_ = attackData;
    timeline_   = timeline;
    data_       = data;
}

void PlayerComboAttackTimelineTrackBuilder::SetupDefaultTracks() {
    if (!attackData_ || !timeline_ || !data_)
        return;

    auto& attackParam = attackData_->GetAttackParam();

    // 総フレーム数を計算して設定
    int32_t totalFrames = CalculateTotalFrames();
    timeline_->SetEndFrame(totalFrames);

    // コライダートラック
    {
        int32_t trackIdx = timeline_->AddTrack("コライダー");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::COLLISION, trackIdx);

        int32_t startFrame     = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.startTime);
        int32_t durationFrames = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.adaptTime);

        timeline_->AddKeyFrame(trackIdx, startFrame, 1.0f,
            static_cast<float>(durationFrames), "コライダー適応時間");
    }

    // 移動イージングトラック
    {
        int32_t trackIdx = timeline_->AddTrack("移動イージング");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::MOVE_EASING, trackIdx);

        int32_t moveStartFrame = KetaEngine::Frame::TimeToFrame(attackParam.moveParam.startTime);
        int32_t moveDuration   = KetaEngine::Frame::TimeToFrame(attackParam.moveParam.easeTime);

        timeline_->AddKeyFrame(trackIdx, moveStartFrame, 1.0f,
            static_cast<float>(moveDuration), "移動イージング時間");
    }

    // 終了待機時間トラック
    {
        int32_t trackIdx = timeline_->AddTrack("終了待機時間");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT, trackIdx);

        int32_t moveEndFrame = KetaEngine::Frame::TimeToFrame(
            attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset);

        int32_t waitDuration = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.finishWaitTime);

        timeline_->AddKeyFrame(trackIdx, moveEndFrame, 1.0f,
            static_cast<float>(waitDuration), "終了待機時間");
    }

    // キャンセル開始トラック
    if (attackParam.timingParam.isCancel) {
        int32_t trackIdx = timeline_->AddTrack("キャンセル開始");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::CANCEL_START, trackIdx);

        int32_t cancelFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.cancelTime);
        timeline_->AddKeyFrame(trackIdx, cancelFrame, 1.0f,
            static_cast<float>(totalFrames - cancelFrame), "キャンセル可能範囲");
    }

    // 先行入力開始トラック
    {
        int32_t trackIdx = timeline_->AddTrack("先行入力開始");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::PRECEDE_INPUT_START, trackIdx);

        int32_t precedeFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.precedeInputTime);
        timeline_->AddKeyFrame(trackIdx, precedeFrame, 1.0f,
            static_cast<float>(totalFrames - precedeFrame), "先行入力可能範囲");
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupRenditionTracks() {
    if (!attackData_ || !timeline_ || !data_)
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

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type          = static_cast<PlayerComboAttackTimelineData::TrackType>(i);
        info.trackIndex    = trackIdx;
        info.fileName      = param.fileName;
        info.isCameraReset = param.isCameraReset;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f,
            "使用ファイル:" + param.fileName);
    }

    // ヒット時演出
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamOnHitFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = std::string(PlayerAttackRenditionData::kRenditionTypeInfos[i].label) + " (ヒット時)";
        int32_t trackIdx      = timeline_->AddTrack(trackName);

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type = static_cast<PlayerComboAttackTimelineData::TrackType>(
            static_cast<int>(PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT) + i);
        info.trackIndex    = trackIdx;
        info.fileName      = param.fileName;
        info.isCameraReset = param.isCameraReset;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f,
            "使用ファイル:" + param.fileName);
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupObjectAnimationTracks() {
    if (!attackData_ || !timeline_ || !data_)
        return;

    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& param = renditionData.GetObjAnimationParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kObjAnimationTypeInfos[i].label;
        int32_t trackIdx      = timeline_->AddTrack(trackName);

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type = static_cast<PlayerComboAttackTimelineData::TrackType>(
            static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f,
            "使用ファイル:" + param.fileName);
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupAudioTracks() {
    if (!attackData_ || !timeline_ || !data_)
        return;

    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::AudioType::Count); ++i) {
        const auto& param = renditionData.GetAudioParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kAudioTypeInfos[i].label;
        int32_t trackIdx      = timeline_->AddTrack(trackName);

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type = static_cast<PlayerComboAttackTimelineData::TrackType>(
            static_cast<int>(PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timeline_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f,
            "使用ファイル:" + param.fileName);
    }
}

void PlayerComboAttackTimelineTrackBuilder::AddTrack(PlayerComboAttackTimelineData::TrackType type) {
    if (!timeline_ || !data_) {
        return;
    }

    if (data_->IsTrackTypeAlreadyAdded(type)) {
        return;
    }

    std::string trackName = data_->GetTrackTypeName(type);
    int32_t trackIdx      = timeline_->AddTrack(trackName);

    PlayerComboAttackTimelineData::TrackInfo info;
    info.type       = type;
    info.trackIndex = trackIdx;
    info.fileName   = "";
    data_->AddTrackInfo(info);

    // 初期キーフレームを追加
    timeline_->AddKeyFrame(trackIdx, 0, 0.0f, 1.0f, "ファイル未選択");
}

void PlayerComboAttackTimelineTrackBuilder::RemoveTrack(int32_t trackIndex) {
    if (!timeline_ || !data_) {
        return;
    }

    data_->RemoveTrackInfo(trackIndex);
    timeline_->RemoveTrack(trackIndex);
}

int32_t PlayerComboAttackTimelineTrackBuilder::CalculateTotalFrames() const {
    if (!attackData_) {
        return 0;
    }

    auto& attackParam = attackData_->GetAttackParam();

    float totalTime = attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset + attackParam.timingParam.finishWaitTime;

    return KetaEngine::Frame::TimeToFrame(totalTime);
}