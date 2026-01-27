#include "PlayerComboAttackTimelineTrackBuilder.h"
#include "../PlayerAttackRenditionData.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"

void PlayerComboAttackTimelineTrackBuilder::Init(
    PlayerComboAttackData* attackData,
    KetaEngine::TimelineDrawer* timeline,
    PlayerComboAttackTimelineData* data) {

    attackData_     = attackData;
    timelineDrawer_ = timeline;
    data_           = data;
}

void PlayerComboAttackTimelineTrackBuilder::SetupDefaultTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_)
        return;

    auto& attackParam = attackData_->GetAttackParam();

    // 総フレーム数を計算して設定
    int32_t totalFrames = CalculateTotalFrames();
    timelineDrawer_->SetEndFrame(totalFrames);

    // コライダートラック
    {
        int32_t trackIdx = timelineDrawer_->AddTrack("コライダー");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::COLLISION, trackIdx);

        int32_t startFrame     = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.startTime);
        int32_t durationFrames = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.adaptTime);

        timelineDrawer_->AddKeyFrame(trackIdx, startFrame, 1.0f,
            static_cast<float>(durationFrames), "コライダー適応時間");
    }

    // 移動イージングトラック
    {
        int32_t trackIdx = timelineDrawer_->AddTrack("移動イージング");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::MOVE_EASING, trackIdx);

        int32_t moveStartFrame = KetaEngine::Frame::TimeToFrame(attackParam.moveParam.startTime);
        int32_t moveDuration   = KetaEngine::Frame::TimeToFrame(attackParam.moveParam.easeTime);

        timelineDrawer_->AddKeyFrame(trackIdx, moveStartFrame, 1.0f,
            static_cast<float>(moveDuration), "移動イージング時間");
    }

    // 終了待機時間トラック
    {
        int32_t trackIdx = timelineDrawer_->AddTrack("終了待機時間");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT, trackIdx);

        int32_t moveEndFrame = KetaEngine::Frame::TimeToFrame(
            attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset);

        int32_t waitDuration = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.finishWaitTime);

        timelineDrawer_->AddKeyFrame(trackIdx, moveEndFrame, 1.0f,
            static_cast<float>(waitDuration), "終了待機時間");
    }

    // キャンセル開始トラック
    if (attackParam.timingParam.isCancel) {
        int32_t trackIdx = timelineDrawer_->AddTrack("キャンセル開始");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::CANCEL_START, trackIdx);

        int32_t cancelFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.cancelTime);
        timelineDrawer_->AddKeyFrame(trackIdx, cancelFrame, 1.0f,
            static_cast<float>(totalFrames - cancelFrame), "キャンセル可能範囲");
    }

    // 先行入力開始トラック
    {
        int32_t trackIdx = timelineDrawer_->AddTrack("先行入力開始");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::PRECEDE_INPUT_START, trackIdx);

        int32_t precedeFrame = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.precedeInputTime);
        timelineDrawer_->AddKeyFrame(trackIdx, precedeFrame, 1.0f,
            static_cast<float>(totalFrames - precedeFrame), "先行入力可能範囲");
    }
}

template <typename ParamType, typename InfoArrayType>
void PlayerComboAttackTimelineTrackBuilder::SetupTracksGeneric(
    const InfoArrayType& typeInfos,
    int32_t count,
    PlayerComboAttackTimelineData::TrackType baseTrackType,
    const char* suffix) {

    if (!attackData_ || !timelineDrawer_ || !data_)
        return;

    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < count; ++i) {
        // パラメータ取得
        const ParamType* param = nullptr;
        if constexpr (std::is_same_v<ParamType, PlayerAttackRenditionData::RenditionParam>) {
            param = suffix ? &renditionData.GetRenditionParamOnHitFromIndex(i)
                           : &renditionData.GetRenditionParamFromIndex(i);
        } else if constexpr (std::is_same_v<ParamType, PlayerAttackRenditionData::ObjAnimationParam>) {
            param = &renditionData.GetObjAnimationParamFromIndex(i);
        } else if constexpr (std::is_same_v<ParamType, PlayerAttackRenditionData::AudioParam>) {
            param = &renditionData.GetAudioParamFromIndex(i);
        }

        if (!param || param->fileName.empty() || param->fileName == "None") {
            continue;
        }

        // トラック名作成
        std::string trackName = typeInfos[i].label;
        if (suffix) {
            trackName += suffix;
        }

        int32_t trackIdx = timelineDrawer_->AddTrack(trackName);

        // トラック情報設定
        PlayerComboAttackTimelineData::TrackInfo info;
        info.type       = static_cast<PlayerComboAttackTimelineData::TrackType>(static_cast<int>(baseTrackType) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param->fileName;

        // isCameraResetはRenditionParamの場合のみ
        if constexpr (std::is_same_v<ParamType, PlayerAttackRenditionData::RenditionParam>) {
            info.isCameraReset = param->isCameraReset;
        }

        data_->AddTrackInfo(info);

        // キーフレーム追加
        int32_t frame = KetaEngine::Frame::TimeToFrame(param->startTiming);
        timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + param->fileName);
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupRenditionTracks() {
    // 通常演出
    SetupTracksGeneric<PlayerAttackRenditionData::RenditionParam>(
        PlayerAttackRenditionData::kRenditionTypeInfos,
        static_cast<int32_t>(PlayerAttackRenditionData::Type::Count),
        PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION,
        nullptr);

    // ヒット時演出
    SetupTracksGeneric<PlayerAttackRenditionData::RenditionParam>(
        PlayerAttackRenditionData::kRenditionTypeInfos,
        static_cast<int32_t>(PlayerAttackRenditionData::Type::Count),
        PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT,
        " (ヒット時)");
}

void PlayerComboAttackTimelineTrackBuilder::SetupObjectAnimationTracks() {
    SetupTracksGeneric<PlayerAttackRenditionData::ObjAnimationParam>(
        PlayerAttackRenditionData::kObjAnimationTypeInfos,
        static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count),
        PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD,
        nullptr);
}

void PlayerComboAttackTimelineTrackBuilder::SetupAudioTracks() {
    SetupTracksGeneric<PlayerAttackRenditionData::AudioParam>(
        PlayerAttackRenditionData::kAudioTypeInfos,
        static_cast<int32_t>(PlayerAttackRenditionData::AudioType::Count),
        PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK,
        nullptr);
}

void PlayerComboAttackTimelineTrackBuilder::AddTrack(PlayerComboAttackTimelineData::TrackType type) {
    if (!timelineDrawer_ || !data_) {
        return;
    }

    if (data_->IsTrackTypeAlreadyAdded(type)) {
        return;
    }

    std::string trackName = data_->GetTrackTypeName(type);
    int32_t trackIdx      = timelineDrawer_->AddTrack(trackName);

    PlayerComboAttackTimelineData::TrackInfo info;
    info.type       = type;
    info.trackIndex = trackIdx;
    info.fileName   = "";
    data_->AddTrackInfo(info);

    // 初期キーフレームを追加
    timelineDrawer_->AddKeyFrame(trackIdx, 0, 0.0f, 1.0f, "ファイル未選択");
}

void PlayerComboAttackTimelineTrackBuilder::RemoveTrack(int32_t trackIndex) {
    if (!timelineDrawer_ || !data_) {
        return;
    }

    data_->RemoveTrackInfo(trackIndex);
    timelineDrawer_->RemoveTrack(trackIndex);
}

int32_t PlayerComboAttackTimelineTrackBuilder::CalculateTotalFrames() const {
    if (!attackData_) {
        return 0;
    }

    auto& attackParam = attackData_->GetAttackParam();

    float totalTime = attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset + attackParam.timingParam.finishWaitTime;

    return KetaEngine::Frame::TimeToFrame(totalTime);
}