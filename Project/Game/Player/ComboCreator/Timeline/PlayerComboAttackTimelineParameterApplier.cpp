#include "PlayerComboAttackTimelineParameterApplier.h"
#include "../ComboBranchParameter.h"
#include "../PlayerAttackRenditionData.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"
#include <algorithm>

void PlayerComboAttackTimelineParameterApplier::Init(
    PlayerComboAttackData* attackData,
    KetaEngine::TimelineDrawer* timeline,
    PlayerComboAttackTimelineData* data) {

    attackData_     = attackData;
    timelineDrawer_ = timeline;
    timeLineData_   = data;
}

///==========================================================
/// トラックからキーフレーム情報を取得する共通関数
///==========================================================
std::optional<PlayerComboAttackTimelineParameterApplier::KeyFrameInfo>
PlayerComboAttackTimelineParameterApplier::GetKeyFrameInfo(
    PlayerComboAttackTimelineData::DefaultTrack track) const {

    if (!timelineDrawer_ || !timeLineData_) {
        return std::nullopt;
    }

    // トラックインデックスを取得
    int32_t trackIdx = timeLineData_->GetDefaultTrackIndex(track);
    if (trackIdx < 0) {
        return std::nullopt;
    }

    // トラックデータを取得
    const auto& tracks = timelineDrawer_->GetTracks();
    if (trackIdx >= static_cast<int32_t>(tracks.size())) {
        return std::nullopt;
    }

    // キーフレームを取得
    const auto& keyframes = tracks[trackIdx].keyframes;
    if (keyframes.empty()) {
        return std::nullopt;
    }

    // キーフレーム情報を返す
    KeyFrameInfo info;
    info.startFrame = keyframes[0].frame;
    info.duration   = keyframes[0].duration;
    return info;
}

///==========================================================
/// 単一フレーム値を取得する共通関数
///==========================================================
std::optional<int32_t>
PlayerComboAttackTimelineParameterApplier::GetSingleFrameValue(
    PlayerComboAttackTimelineData::DefaultTrack track) const {

    if (!timelineDrawer_ || !timeLineData_) {
        return std::nullopt;
    }

    int32_t trackIdx = timeLineData_->GetDefaultTrackIndex(track);
    if (trackIdx < 0) {
        return std::nullopt;
    }

    int32_t frame = timelineDrawer_->GetFirstKeyFrameFrame(trackIdx);
    return frame;
}

///==========================================================
/// パラメータ適用のヘルパー関数
///==========================================================

// KeyFrameInfoを使った処理を実行
template <typename Func>
void ApplyIfPresent(const std::optional<PlayerComboAttackTimelineParameterApplier::KeyFrameInfo>& opt, Func&& func) {
    if (opt) {
        func(*opt);
    }
}

// 単一フレーム値を使った処理を実行
template <typename Func>
void ApplyIfPresent(const std::optional<int32_t>& opt, Func&& func) {
    if (opt) {
        func(*opt);
    }
}

///==========================================================
/// パラメータ適用
///==========================================================
void PlayerComboAttackTimelineParameterApplier::ApplyToParameters() {
    if (!attackData_ || !timelineDrawer_ || !timeLineData_) {
        return;
    }

    // 攻撃パラメータを取得
    auto& attackParam = attackData_->GetAttackParam();

    // コライダー適用
    ApplyIfPresent(GetKeyFrameInfo(PlayerComboAttackTimelineData::DefaultTrack::COLLISION), [&](const KeyFrameInfo& info) {
        attackParam.collisionParam.startTime = KetaEngine::Frame::FrameToTime(info.startFrame);
        attackParam.collisionParam.adaptTime = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(info.duration));
    });

    // 移動イージング適用
    ApplyIfPresent(GetKeyFrameInfo(PlayerComboAttackTimelineData::DefaultTrack::MOVE_EASING), [&](const KeyFrameInfo& info) {
        attackParam.moveParam.startTime = KetaEngine::Frame::FrameToTime(info.startFrame);
        attackParam.moveParam.easeTime  = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(info.duration));
    });

    // 終了待機時間の適用
    ApplyIfPresent(GetKeyFrameInfo(PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT), [&](const KeyFrameInfo& info) {
        attackParam.timingParam.finishWaitTime = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(info.duration));

        // 終了待機の開始位置から移動終了位置を引いてfinishTimeOffsetを計算
        ApplyIfPresent(GetKeyFrameInfo(PlayerComboAttackTimelineData::DefaultTrack::MOVE_EASING),
            [&](const KeyFrameInfo& moveInfo) {
                int32_t waitStartFrame                 = info.startFrame;
                int32_t moveEndFrame                   = moveInfo.startFrame + static_cast<int32_t>(moveInfo.duration);
                int32_t offsetFrames                   = waitStartFrame - moveEndFrame;
                attackParam.moveParam.finishTimeOffset = KetaEngine::Frame::FrameToTime(offsetFrames);
            });
    });

    // 演出系とコンボ分岐タイミングの適用
    auto& branches = attackData_->GetComboBranches();
    for (const auto& trackInfo : timeLineData_->GetAddedTracks()) {
        int32_t frame = timelineDrawer_->GetFirstKeyFrameFrame(trackInfo.trackIndex);
        float timing  = KetaEngine::Frame::FrameToTime(frame);

        // コンボ分岐のタイミング
        if (trackInfo.branchIndex >= 0 && trackInfo.branchIndex < static_cast<int32_t>(branches.size())) {
            auto& branch = branches[trackInfo.branchIndex];

            if (trackInfo.type == PlayerComboAttackTimelineData::TrackType::CANCEL_TIME) {
                branch->SetCancelTime(timing);
            } else if (trackInfo.type == PlayerComboAttackTimelineData::TrackType::PRECEDE_INPUT) {
                branch->SetPrecedeInputTime(timing);
            }
        } else {
            // 演出系の適用
            ApplyTrackToRendition(trackInfo, timing);
        }
    }

    // 終了待機時間のキーフレーム位置を更新
    UpdateFinishWaitKeyFramePosition();

    // タイムラインの終了フレームを再計算して更新
    UpdateTimelineEndFrame();
}

void PlayerComboAttackTimelineParameterApplier::UpdateFinishWaitKeyFramePosition() {
    if (!timelineDrawer_ || !timeLineData_) {
        return;
    }

    int32_t finishWaitTrackIdx = timeLineData_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT);
    int32_t moveTrackIdx = timeLineData_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::MOVE_EASING);

    if (finishWaitTrackIdx < 0 || moveTrackIdx < 0)
        return;

    auto& tracks = timelineDrawer_->GetTracks();
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
    if (!attackData_ || !timelineDrawer_)
        return;

    int32_t totalFrames = CalculateTotalFrames();
    timelineDrawer_->SetEndFrame(totalFrames);
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

    // 通常演出
    if (typeInt >= static_cast<int>(PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION) && typeInt <= static_cast<int>(PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK)) {

        auto& param = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamFromIndex(typeInt));
        param.fileName      = trackInfo.fileName;
        param.startTiming   = timing;
        param.isCameraReset = trackInfo.isCameraReset;
        param.volume        = trackInfo.volume;

    }
    // ヒット時演出
    else if (typeInt >= static_cast<int>(PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT) && typeInt <= static_cast<int>(PlayerComboAttackTimelineData::TrackType::AUDIO_ATTACK_ON_HIT)) {

        int baseIndex = typeInt - static_cast<int>(PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT);
        auto& param   = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamOnHitFromIndex(baseIndex));
        param.fileName      = trackInfo.fileName;
        param.startTiming   = timing;
        param.isCameraReset = trackInfo.isCameraReset;
        param.volume        = trackInfo.volume;

    }
    // オブジェクトアニメーション
    else if (typeInt >= static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_MAIN_HEAD)) {

        int baseIndex = typeInt - static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD);
        auto& param   = const_cast<PlayerAttackRenditionData::ObjAnimationParam&>(
            renditionData.GetObjAnimationParamFromIndex(baseIndex));
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