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
    {
        int32_t trackIdx = timeLineData_->GetDefaultTrackIndex(
            PlayerComboAttackTimelineData::DefaultTrack::COLLISION);

        if (trackIdx >= 0 && trackIdx < static_cast<int32_t>(timelineDrawer_->GetTracks().size())) {
            auto& keyframes = timelineDrawer_->GetTracks()[trackIdx].keyframes;

            if (!keyframes.empty()) {
                // 先頭キーフレームの位置 → startTime
                int32_t startFrame = keyframes[0].frame;
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
                int32_t durationFrames = static_cast<int32_t>(newDurFrames);
                attackParam.collisionParam.adaptTime = KetaEngine::Frame::FrameToTime(durationFrames);

                // 全キーフレームを再配置（先頭位置 + i * (duration + wait)）
                int32_t waitFrames = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.loopWaitTime);
                for (int32_t i = 0; i < static_cast<int32_t>(keyframes.size()); ++i) {
                    keyframes[i].frame    = startFrame + i * (durationFrames + waitFrames);
                    keyframes[i].duration = static_cast<float>(durationFrames);
                }
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

    // ポストエフェクト・パーティクルエフェクトリストをクリアして再構築
    auto& renditionDataForClear = attackData_->GetRenditionDataForPhase(phase_);
    renditionDataForClear.ClearPostEffectList();
    renditionDataForClear.ClearPostEffectOnHitList();
    renditionDataForClear.ClearParticleEffectList();
    renditionDataForClear.ClearParticleEffectOnHitList();

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
    int32_t collisionTrackIdx = timeLineData_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::COLLISION);

    if (finishWaitTrackIdx < 0 || moveTrackIdx < 0 || collisionTrackIdx < 0)
        return;

    auto& tracks = timelineDrawer_->GetTracks();
    if (finishWaitTrackIdx >= static_cast<int32_t>(tracks.size()) ||
        moveTrackIdx >= static_cast<int32_t>(tracks.size()) ||
        collisionTrackIdx >= static_cast<int32_t>(tracks.size()))
        return;

    const auto& moveKeyframes      = tracks[moveTrackIdx].keyframes;
    const auto& collisionKeyframes = tracks[collisionTrackIdx].keyframes;
    auto& finishWaitKeyframes      = tracks[finishWaitTrackIdx].keyframes;

    if (moveKeyframes.empty() || finishWaitKeyframes.empty())
        return;

    int32_t moveEndFrame = moveKeyframes[0].frame + static_cast<int32_t>(moveKeyframes[0].duration);

    // ループがある場合は最後のキーフレームの終端を使用
    int32_t collisionEndFrame = !collisionKeyframes.empty()
        ? collisionKeyframes.back().frame + static_cast<int32_t>(collisionKeyframes.back().duration)
        : 0;

    finishWaitKeyframes[0].frame = std::max(moveEndFrame, collisionEndFrame);
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

    auto& attackParam = attackData_->GetAttackParamForPhase(phase_);

    int32_t collisionEndFrame = KetaEngine::Frame::TimeToFrame(
        attackParam.collisionParam.startTime + attackParam.collisionParam.adaptTime);

    int32_t moveEndFrame = KetaEngine::Frame::TimeToFrame(
        attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset);

    return std::max(collisionEndFrame, moveEndFrame);
}


static int32_t GetRenditionIndexFromTrackType(PlayerComboAttackTimelineData::TrackType type) {
    using TT = PlayerComboAttackTimelineData::TrackType;
    using RT = PlayerAttackRenditionData::Type;
    switch (type) {
    case TT::CAMERA_ACTION:   return static_cast<int32_t>(RT::CameraAction);
    case TT::HIT_STOP:        return static_cast<int32_t>(RT::HitStop);
    case TT::SHAKE_ACTION:    return static_cast<int32_t>(RT::ShakeAction);
    case TT::POST_EFFECT:     return static_cast<int32_t>(RT::PostEffect);
    case TT::PARTICLE_EFFECT: return static_cast<int32_t>(RT::ParticleEffect);
    case TT::AUDIO_ATTACK:    return static_cast<int32_t>(RT::AudioAttack);
    default:                  return -1;
    }
}

static int32_t GetRenditionOnHitIndexFromTrackType(PlayerComboAttackTimelineData::TrackType type) {
    using TT = PlayerComboAttackTimelineData::TrackType;
    using RT = PlayerAttackRenditionData::Type;
    switch (type) {
    case TT::CAMERA_ACTION_ON_HIT:   return static_cast<int32_t>(RT::CameraAction);
    case TT::HIT_STOP_ON_HIT:        return static_cast<int32_t>(RT::HitStop);
    case TT::SHAKE_ACTION_ON_HIT:    return static_cast<int32_t>(RT::ShakeAction);
    case TT::POST_EFFECT_ON_HIT:     return static_cast<int32_t>(RT::PostEffect);
    case TT::PARTICLE_EFFECT_ON_HIT: return static_cast<int32_t>(RT::ParticleEffect);
    case TT::AUDIO_ATTACK_ON_HIT:    return static_cast<int32_t>(RT::AudioAttack);
    default:                         return -1;
    }
}

void PlayerComboAttackTimelineParameterApplier::ApplyTrackToRendition(
    const PlayerComboAttackTimelineData::TrackInfo& trackInfo, float timing) {

    if (!attackData_) {
        return;
    }

    using TT = PlayerComboAttackTimelineData::TrackType;
    auto& renditionData = attackData_->GetRenditionDataForPhase(phase_);
    int typeInt         = static_cast<int>(trackInfo.type);

    // 通常演出
    if (typeInt >= static_cast<int>(TT::CAMERA_ACTION) && typeInt <= static_cast<int>(TT::AUDIO_ATTACK)) {
        // ポストエフェクトは複数対応：リストに追加
        if (trackInfo.type == TT::POST_EFFECT) {
            PlayerAttackRenditionData::RenditionParam param;
            param.fileName    = trackInfo.fileName;
            param.startTiming = timing;
            renditionData.AddPostEffect(param);
            return;
        }
        // パーティクルエフェクトは複数対応：リストに追加
        if (trackInfo.type == TT::PARTICLE_EFFECT) {
            PlayerAttackRenditionData::RenditionParam param;
            param.fileName    = trackInfo.fileName;
            param.startTiming = timing;
            renditionData.AddParticleEffect(param);
            return;
        }

        int32_t rendIdx = GetRenditionIndexFromTrackType(trackInfo.type);
        if (rendIdx < 0) return;

        auto& param = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamFromIndex(rendIdx));
        param.fileName      = trackInfo.fileName;
        param.startTiming   = timing;
        param.isCameraReset = trackInfo.isCameraReset;
        param.volume        = trackInfo.volume;
    }
    // ヒット時演出
    else if (typeInt >= static_cast<int>(TT::CAMERA_ACTION_ON_HIT) && typeInt <= static_cast<int>(TT::AUDIO_ATTACK_ON_HIT)) {
        // ヒット時ポストエフェクトは複数対応
        if (trackInfo.type == TT::POST_EFFECT_ON_HIT) {
            PlayerAttackRenditionData::RenditionParam p;
            p.fileName    = trackInfo.fileName;
            p.startTiming = timing;
            renditionData.AddPostEffectOnHit(p);
            return;
        }
        // ヒット時パーティクルエフェクトは複数対応
        if (trackInfo.type == TT::PARTICLE_EFFECT_ON_HIT) {
            PlayerAttackRenditionData::RenditionParam p;
            p.fileName    = trackInfo.fileName;
            p.startTiming = timing;
            renditionData.AddParticleEffectOnHit(p);
            return;
        }

        int32_t rendIdx = GetRenditionOnHitIndexFromTrackType(trackInfo.type);
        if (rendIdx < 0) return;

        auto& param = const_cast<PlayerAttackRenditionData::RenditionParam&>(
            renditionData.GetRenditionParamOnHitFromIndex(rendIdx));
        param.fileName       = trackInfo.fileName;
        param.startTiming    = timing;
        param.isCameraReset  = trackInfo.isCameraReset;
        param.volume         = trackInfo.volume;
        param.repeatOnDamage = trackInfo.repeatOnDamage;
    }
    // オブジェクトアニメーション
    else if (typeInt >= static_cast<int>(TT::OBJ_ANIM_HEAD) && typeInt <= static_cast<int>(TT::OBJ_ANIM_MAIN_HEAD)) {
        int baseIndex = typeInt - static_cast<int>(TT::OBJ_ANIM_HEAD);
        auto& param   = const_cast<PlayerAttackRenditionData::ObjAnimationParam&>(
            renditionData.GetObjAnimationParamFromIndex(baseIndex));
        param.fileName      = trackInfo.fileName;
        param.startTiming   = timing;
        param.trailFileName = trackInfo.trailFileName;
    }
    // MainHead追従トレイル
    else if (trackInfo.type == TT::RIBBON_TRAIL_MAIN_HEAD) {
        auto& param = const_cast<PlayerAttackRenditionData::ObjAnimationParam&>(
            renditionData.GetObjAnimationParamFromType(PlayerAttackRenditionData::ObjAnimationType::MainHead));
        param.trailFileName = trackInfo.fileName;
    }
    // コントローラ振動
    else if (trackInfo.type == TT::VIBRATION) {
        const auto& tracks = timelineDrawer_->GetTracks();
        if (trackInfo.trackIndex < static_cast<int32_t>(tracks.size())) {
            const auto& keyframes = tracks[trackInfo.trackIndex].keyframes;
            if (!keyframes.empty()) {
                float duration = KetaEngine::Frame::FrameToTime(static_cast<int32_t>(keyframes[0].duration));
                auto& vibParam = renditionData.GetVibrationParam();
                vibParam.startTiming    = timing;
                vibParam.duration       = duration;
                vibParam.intensity      = trackInfo.vibrationIntensity;
                vibParam.triggerByHit   = trackInfo.triggerByHit;
                vibParam.repeatOnDamage = trackInfo.repeatOnDamage;
            }
        }
    }
}

int32_t PlayerComboAttackTimelineParameterApplier::CalculateTotalFrames() const {
    if (!attackData_) {
        return 0;
    }

    auto& attackParam = attackData_->GetAttackParamForPhase(phase_);

    float totalTime = attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset + attackParam.timingParam.finishWaitTime;

    return KetaEngine::Frame::TimeToFrame(totalTime);
}