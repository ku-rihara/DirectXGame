#include "PlayerComboAttackTimelineParameterApplier.h"
#include "../ComboBranchParameter.h"
#include "../PlayerAttackRenditionData.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"
#include <algorithm>

void PlayerComboAttackTimelineParameterApplier::Init(
    PlayerComboAttackData* attackData,
    KetaEngine::TimelineDrawer* timeline,
    PlayerComboAttackTimelineData* data,
    AttackTimelinePhase phase) {

    // メンバーポインタを保存
    attackData_     = attackData;
    timelineDrawer_ = timeline;
    timeLineData_   = data;
    phase_          = phase;
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

    // フェーズ別攻撃パラメータを取得
    auto& attackParam = attackData_->GetAttackParamForPhase(phase_);

    // コライダー適用
    int32_t trackIdx = timeLineData_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::COLLISION);

    if (trackIdx >= 0 && trackIdx < static_cast<int32_t>(timelineDrawer_->GetTracks().size())) {
        auto& keyframes = timelineDrawer_->GetTracks()[trackIdx].keyframes;

        if (!keyframes.empty()) {
            // 先頭キーフレームの位置 → startTime
            int32_t startFrame                   = keyframes[0].frame;
            attackParam.collisionParam.startTime = KetaEngine::Frame::FrameToTime(startFrame);

            // いずれかのdurationが変更されていたらそれを共通durationとして採用
            float storedDurFrames = static_cast<float>(
                KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.adaptTime));
            float newDurFrames = keyframes[0].duration;
            for (const auto& kf : keyframes) {
                if (std::abs(kf.duration - storedDurFrames) > 0.5f) {
                    newDurFrames = kf.duration;
                    break;
                }
            }
            int32_t durationFrames               = static_cast<int32_t>(newDurFrames);
            attackParam.collisionParam.adaptTime = KetaEngine::Frame::FrameToTime(durationFrames);

            // 全キーフレームを再配置（先頭位置 + i * (duration + wait)）
            int32_t waitFrames = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.loopWaitTime);
            for (int32_t i = 0; i < static_cast<int32_t>(keyframes.size()); ++i) {
                keyframes[i].frame    = startFrame + i * (durationFrames + waitFrames);
                keyframes[i].duration = static_cast<float>(durationFrames);
            }
        }
    }

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

    // 演出系リストを全種別クリアしてから再構築
    auto& renditionDataForClear = attackData_->GetRenditionDataForPhase(phase_);
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        auto type = static_cast<PlayerAttackRenditionData::Type>(i);
        renditionDataForClear.ClearRenditionList(type);
        renditionDataForClear.ClearRenditionOnHitList(type);
    }
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        renditionDataForClear.ClearObjAnimationList(static_cast<PlayerAttackRenditionData::ObjAnimationType>(i));
    }
    renditionDataForClear.ClearVibrationList();

    // 演出系とコンボ分岐タイミングの適用
    auto& branches = attackData_->GetComboBranches();
    for (const auto& trackInfo : timeLineData_->GetAddedTracks()) {
        // コンボ分岐のタイミング
        if (trackInfo.branchIndex >= 0 && trackInfo.branchIndex < static_cast<int32_t>(branches.size())) {
            int32_t frame = timelineDrawer_->GetFirstKeyFrameFrame(trackInfo.trackIndex);
            float timing  = KetaEngine::Frame::FrameToTime(frame);
            auto& branch  = branches[trackInfo.branchIndex];

            if (trackInfo.type == PlayerComboAttackTimelineData::TrackType::CANCEL_TIME) {
                branch->SetCancelTime(timing);
            } else if (trackInfo.type == PlayerComboAttackTimelineData::TrackType::PRECEDE_INPUT) {
                branch->SetPrecedeInputTime(timing);
            }
        } else {
            // 演出系の適用
            ApplyTrackToRendition(trackInfo);
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

    // デフォルトトラックのインデックスを取得
    int32_t finishWaitTrackIdx = timeLineData_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT);
    int32_t moveTrackIdx = timeLineData_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::MOVE_EASING);
    int32_t collisionTrackIdx = timeLineData_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::COLLISION);

    // トラックが存在しない場合は処理しない
    if (finishWaitTrackIdx < 0 || moveTrackIdx < 0 || collisionTrackIdx < 0) {
        return;
    }

    auto& tracks = timelineDrawer_->GetTracks();
    // トラックインデックスが範囲外の場合も処理しない
    if (finishWaitTrackIdx >= static_cast<int32_t>(tracks.size()) || moveTrackIdx >= static_cast<int32_t>(tracks.size()) || collisionTrackIdx >= static_cast<int32_t>(tracks.size())) {
        return;
    }

    // キーフレームデータを取得
    const auto& moveKeyframes      = tracks[moveTrackIdx].keyframes;
    const auto& collisionKeyframes = tracks[collisionTrackIdx].keyframes;
    auto& finishWaitKeyframes      = tracks[finishWaitTrackIdx].keyframes;

    // 移動とコリジョンのキーフレームが存在しない場合は終了待機の位置を更新できないため処理しない
    if (moveKeyframes.empty() || finishWaitKeyframes.empty()) {
        return;
    }

    // 移動の終了フレームを計算（開始フレーム + duration）
    int32_t moveEndFrame = moveKeyframes[0].frame + static_cast<int32_t>(moveKeyframes[0].duration);

    // ループがある場合は最後のキーフレームの終端を使用
    int32_t collisionEndFrame = !collisionKeyframes.empty()
                                    ? collisionKeyframes.back().frame + static_cast<int32_t>(collisionKeyframes.back().duration)
                                    : 0;

    // 終了待機の開始フレームを移動終了とコリジョン終了の遅い方に設定
    finishWaitKeyframes[0].frame = std::max(moveEndFrame, collisionEndFrame);
}

void PlayerComboAttackTimelineParameterApplier::UpdateTimelineEndFrame() {
    if (!attackData_ || !timelineDrawer_) {
        return;
    }

    // タイムラインの終了フレームの計算
    int32_t totalFrames = CalculateTotalFrames();
    timelineDrawer_->SetEndFrame(totalFrames);
}

int32_t PlayerComboAttackTimelineParameterApplier::GetFinishWaitStartFrame() const {
    if (!attackData_) {
        return 0;
    }

    // コリジョン終了時間と移動終了時間の遅い方を終了待機開始フレームとする
    auto& attackParam = attackData_->GetAttackParamForPhase(phase_);

    // コリジョン終了フレーム = コリジョン開始時間 + 適用時間
    int32_t collisionEndFrame = KetaEngine::Frame::TimeToFrame(
        attackParam.collisionParam.startTime + attackParam.collisionParam.adaptTime);

    // 移動終了フレーム = 移動開始時間 + イージング時間 + 終了待機開始からのオフセット時間
    int32_t moveEndFrame = KetaEngine::Frame::TimeToFrame(
        attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset);

    // 長い方を終了待機の開始フレームとする
    return std::max(collisionEndFrame, moveEndFrame);
}

static int32_t GetRenditionIndexFromTrackType(PlayerComboAttackTimelineData::TrackType type) {
    using TT = PlayerComboAttackTimelineData::TrackType;
    using RT = PlayerAttackRenditionData::Type;
    // 通常演出のトラックタイプをPlayerAttackRenditionDataのTypeにマッピング
    switch (type) {
    case TT::CAMERA_ACTION:
        return static_cast<int32_t>(RT::CameraAction);
    case TT::HIT_STOP:
        return static_cast<int32_t>(RT::HitStop);
    case TT::SHAKE_ACTION:
        return static_cast<int32_t>(RT::ShakeAction);
    case TT::POST_EFFECT:
        return static_cast<int32_t>(RT::PostEffect);
    case TT::PARTICLE_EFFECT:
        return static_cast<int32_t>(RT::ParticleEffect);
    case TT::AUDIO_ATTACK:
        return static_cast<int32_t>(RT::AudioAttack);
    default:
        return -1;
    }
}

static int32_t GetRenditionOnHitIndexFromTrackType(PlayerComboAttackTimelineData::TrackType type) {
    using TT = PlayerComboAttackTimelineData::TrackType;
    using RT = PlayerAttackRenditionData::Type;
    // ヒット時演出のトラックタイプをPlayerAttackRenditionDataのTypeにマッピング
    switch (type) {
    case TT::CAMERA_ACTION_ON_HIT:
        return static_cast<int32_t>(RT::CameraAction);
    case TT::HIT_STOP_ON_HIT:
        return static_cast<int32_t>(RT::HitStop);
    case TT::SHAKE_ACTION_ON_HIT:
        return static_cast<int32_t>(RT::ShakeAction);
    case TT::POST_EFFECT_ON_HIT:
        return static_cast<int32_t>(RT::PostEffect);
    case TT::PARTICLE_EFFECT_ON_HIT:
        return static_cast<int32_t>(RT::ParticleEffect);
    case TT::AUDIO_ATTACK_ON_HIT:
        return static_cast<int32_t>(RT::AudioAttack);
    default:
        return -1;
    }
}

void PlayerComboAttackTimelineParameterApplier::ApplyTrackToRendition(
    const PlayerComboAttackTimelineData::TrackInfo& trackInfo) {

    if (!attackData_ || !timelineDrawer_) {
        return;
    }

    using TT            = PlayerComboAttackTimelineData::TrackType;
    auto& renditionData = attackData_->GetRenditionDataForPhase(phase_);
    int typeInt         = static_cast<int>(trackInfo.type);

    const auto& tracks = timelineDrawer_->GetTracks();
    if (trackInfo.trackIndex < 0 || trackInfo.trackIndex >= static_cast<int32_t>(tracks.size())) {
        return;
    }
    const auto& keyframes = tracks[trackInfo.trackIndex].keyframes;

    // 通常演出
    if (typeInt >= static_cast<int>(TT::CAMERA_ACTION) && typeInt <= static_cast<int>(TT::AUDIO_ATTACK)) {
        int32_t rendIdx = GetRenditionIndexFromTrackType(trackInfo.type);
        if (rendIdx < 0) {
            return;
        }
        auto type = static_cast<PlayerAttackRenditionData::Type>(rendIdx);

        for (const auto& kf : keyframes) {
            PlayerAttackRenditionData::RenditionParam param;
            param.fileName      = trackInfo.fileName;
            param.startTiming   = KetaEngine::Frame::FrameToTime(kf.frame);
            param.isCameraReset = trackInfo.isCameraReset;
            param.volume        = trackInfo.volume;
            renditionData.AddRenditionEntry(type, param);
        }
        return;
    }

    // ヒット時演出
    if (typeInt >= static_cast<int>(TT::CAMERA_ACTION_ON_HIT) && typeInt <= static_cast<int>(TT::AUDIO_ATTACK_ON_HIT)) {
        int32_t rendIdx = GetRenditionOnHitIndexFromTrackType(trackInfo.type);
        if (rendIdx < 0) {
            return;
        }
        auto type = static_cast<PlayerAttackRenditionData::Type>(rendIdx);

        for (const auto& kf : keyframes) {
            PlayerAttackRenditionData::RenditionParam param;
            param.fileName       = trackInfo.fileName;
            param.startTiming    = KetaEngine::Frame::FrameToTime(kf.frame);
            param.isCameraReset  = trackInfo.isCameraReset;
            param.volume         = trackInfo.volume;
            param.repeatOnDamage = trackInfo.repeatOnDamage;
            renditionData.AddRenditionOnHitEntry(type, param);
        }
        return;
    }

    // オブジェクトアニメーション
    if (typeInt >= static_cast<int>(TT::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(TT::OBJ_ANIM_MAIN_HEAD)) {
        int baseIndex  = typeInt - static_cast<int>(TT::OBJ_ANIM_HEAD);
        auto animType  = static_cast<PlayerAttackRenditionData::ObjAnimationType>(baseIndex);

        for (const auto& kf : keyframes) {
            PlayerAttackRenditionData::ObjAnimationParam param;
            param.fileName      = trackInfo.fileName;
            param.startTiming   = KetaEngine::Frame::FrameToTime(kf.frame);
            param.trailFileName = trackInfo.trailFileName;
            renditionData.AddObjAnimationEntry(animType, param);
        }
        return;
    }

    // MainHead追従トレイル
    if (trackInfo.type == TT::RIBBON_TRAIL_MAIN_HEAD) {
        auto mainHeadType    = PlayerAttackRenditionData::ObjAnimationType::MainHead;
        auto existingEntries = renditionData.GetObjAnimationListFromType(mainHeadType);

        if (existingEntries.empty()) {
            PlayerAttackRenditionData::ObjAnimationParam param;
            param.trailFileName = trackInfo.fileName;
            renditionData.AddObjAnimationEntry(mainHeadType, param);
        } else {
            renditionData.ClearObjAnimationList(mainHeadType);
            for (auto& param : existingEntries) {
                param.trailFileName = trackInfo.fileName;
                renditionData.AddObjAnimationEntry(mainHeadType, param);
            }
        }
        return;
    }

    // コントローラ振動
    if (trackInfo.type == TT::VIBRATION) {
        for (const auto& kf : keyframes) {
            PlayerAttackRenditionData::VibrationParam param;
            param.startTiming    = KetaEngine::Frame::FrameToTime(kf.frame);
            param.duration       = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(kf.duration));
            param.intensity      = trackInfo.vibrationIntensity;
            param.triggerByHit   = trackInfo.triggerByHit;
            param.repeatOnDamage = trackInfo.repeatOnDamage;
            renditionData.AddVibrationEntry(param);
        }
        return;
    }
}

int32_t PlayerComboAttackTimelineParameterApplier::CalculateTotalFrames() const {
    if (!attackData_) {
        return 0;
    }

    // 合計フレームの計算
    auto& attackParam = attackData_->GetAttackParamForPhase(phase_);
    float totalTime   = attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset + attackParam.timingParam.finishWaitTime;

    // Frame変換して返す
    return KetaEngine::Frame::TimeToFrame(totalTime);
}