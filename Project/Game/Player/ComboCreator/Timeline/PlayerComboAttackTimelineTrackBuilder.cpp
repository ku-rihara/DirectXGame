#include "PlayerComboAttackTimelineTrackBuilder.h"
#include "../ComboBranchParameter.h"
#include "../PlayerAttackRenditionData.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"
#include "input/InputData.h"
#include <algorithm>
#include <functional>


void PlayerComboAttackTimelineTrackBuilder::Init(
    PlayerComboAttackData* attackData,
    KetaEngine::TimelineDrawer* timeline,
    PlayerComboAttackTimelineData* data,
    AttackTimelinePhase phase) {

    attackData_     = attackData;
    timelineDrawer_ = timeline;
    data_           = data;
    phase_          = phase;
}

void PlayerComboAttackTimelineTrackBuilder::SetupDefaultTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_)
        return;

    auto& attackParam = attackData_->GetAttackParamForPhase(phase_);

    // 総フレーム数を計算して設定
    int32_t totalFrames = CalculateTotalFrames();
    timelineDrawer_->SetEndFrame(totalFrames);

    // コライダートラック（ループ数分のキーフレームを追加）
    {
        int32_t trackIdx = timelineDrawer_->AddTrack("コライダー");
        data_->SetDefaultTrackIndex(PlayerComboAttackTimelineData::DefaultTrack::COLLISION, trackIdx);

        int32_t startFrame     = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.startTime);
        int32_t durationFrames = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.adaptTime);
        int32_t waitFrames     = KetaEngine::Frame::TimeToFrame(attackParam.collisionParam.loopWaitTime);
        int32_t totalCount     = attackParam.collisionParam.loopNum + 1; // 初回 + ループ回数

        for (int32_t i = 0; i < totalCount; ++i) {
            int32_t kfFrame = startFrame + i * (durationFrames + waitFrames);
            timelineDrawer_->AddKeyFrame(trackIdx, kfFrame, 1.0f,
                static_cast<float>(durationFrames), "コライダー適応時間");

            // 先頭以外のキーフレームは位置をロック（先頭に連動して動く）
            if (i > 0) {
                auto& kf            = timelineDrawer_->GetTracks()[trackIdx].keyframes.back();
                kf.isPositionLocked = true;
            }
        }
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

        int32_t collisionEndFrame = KetaEngine::Frame::TimeToFrame(
            attackParam.collisionParam.startTime + attackParam.collisionParam.adaptTime);
        int32_t moveEndFrame = KetaEngine::Frame::TimeToFrame(
            attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset);
        int32_t waitStartFrame = (std::max)(collisionEndFrame, moveEndFrame);

        int32_t waitDuration = KetaEngine::Frame::TimeToFrame(attackParam.timingParam.finishWaitTime);

        timelineDrawer_->AddKeyFrame(trackIdx, waitStartFrame, 1.0f,
            static_cast<float>(waitDuration), "終了待機時間");
    }

    // totalFramesが0以下の場合は最小値を設定
    if (totalFrames <= 0) {
        totalFrames = 1;
    }

    // コンボ分岐トラックはMAINフェーズのみ
    if (phase_ != AttackTimelinePhase::MAIN) {
        return;
    }

    const auto& branches = attackData_->GetComboBranches();
    for (size_t i = 0; i < branches.size(); ++i) {
        const auto& branch = branches[i];
        std::string buttonName = GetButtonDisplayName(branch->GetKeyboardButton(), branch->GetGamepadButton());

        // キャンセル開始トラック
        {
            std::string trackName = "キャンセル開始（" + buttonName + "）";
            int32_t trackIdx = timelineDrawer_->AddTrack(trackName);

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type = PlayerComboAttackTimelineData::TrackType::CANCEL_TIME;
            info.trackIndex = trackIdx;
            info.branchIndex = static_cast<int32_t>(i);
            data_->AddTrackInfo(info);

            int32_t cancelFrame = KetaEngine::Frame::TimeToFrame(branch->GetCancelTime());
            // durationが負にならないように
            float duration = static_cast<float>((std::max)(0, totalFrames - cancelFrame));
            timelineDrawer_->AddKeyFrame(trackIdx, cancelFrame, 1.0f, duration, "キャンセル可能範囲");
        }

        // 先行入力開始トラック
        {
            std::string trackName = "先行入力開始（" + buttonName + "）";
            int32_t trackIdx = timelineDrawer_->AddTrack(trackName);

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type = PlayerComboAttackTimelineData::TrackType::PRECEDE_INPUT;
            info.trackIndex = trackIdx;
            info.branchIndex = static_cast<int32_t>(i);
            data_->AddTrackInfo(info);

            int32_t precedeFrame = KetaEngine::Frame::TimeToFrame(branch->GetPrecedeInputTime());
            // durationが負にならないように
            float duration = static_cast<float>((std::max)(0, totalFrames - precedeFrame));
            timelineDrawer_->AddKeyFrame(trackIdx, precedeFrame, 1.0f, duration, "先行入力可能範囲");
        }
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupRenditionTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_) {
        return;
    }

    auto& renditionData = attackData_->GetRenditionDataForPhase(phase_);

    using RendType  = PlayerAttackRenditionData::Type;
    using TrackType = PlayerComboAttackTimelineData::TrackType;

    // RenditionData::Type → TrackType の正引きマッピング（PostEffect/ParticleEffectはリストから個別処理）
    struct RendToTrack { RendType rendType; TrackType trackType; };
    static constexpr RendToTrack kNormalMapping[] = {
        {RendType::CameraAction,   TrackType::CAMERA_ACTION},
        {RendType::HitStop,        TrackType::HIT_STOP},
        {RendType::ShakeAction,    TrackType::SHAKE_ACTION},
        {RendType::AudioAttack,    TrackType::AUDIO_ATTACK},
    };
    static constexpr RendToTrack kOnHitMapping[] = {
        {RendType::CameraAction,   TrackType::CAMERA_ACTION_ON_HIT},
        {RendType::HitStop,        TrackType::HIT_STOP_ON_HIT},
        {RendType::ShakeAction,    TrackType::SHAKE_ACTION_ON_HIT},
        {RendType::AudioAttack,    TrackType::AUDIO_ATTACK_ON_HIT},
    };

    // 通常演出
    for (const auto& m : kNormalMapping) {
        int32_t rendIdx       = static_cast<int32_t>(m.rendType);
        const auto& param     = renditionData.GetRenditionParamFromIndex(rendIdx);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kRenditionTypeInfos[rendIdx].label;
        int32_t trackIdx      = timelineDrawer_->AddTrack(trackName);

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type          = m.trackType;
        info.trackIndex    = trackIdx;
        info.fileName      = param.fileName;
        info.isCameraReset = param.isCameraReset;
        info.volume        = param.volume;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + param.fileName);
    }

    // ポストエフェクト（複数対応）— リストから直接再構築
    {
        const auto& postEffectList = renditionData.GetPostEffectList();
        for (const auto& param : postEffectList) {
            if (param.fileName.empty() || param.fileName == "None") {
                continue;
            }

            int32_t trackIdx = timelineDrawer_->AddTrack("ポストエフェクト");

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type       = TrackType::POST_EFFECT;
            info.trackIndex = trackIdx;
            info.fileName   = param.fileName;
            data_->AddTrackInfo(info);

            int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
            timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + param.fileName);
        }

        const auto& postEffectOnHitList = renditionData.GetPostEffectOnHitList();
        for (const auto& param : postEffectOnHitList) {
            if (param.fileName.empty() || param.fileName == "None") {
                continue;
            }

            int32_t trackIdx = timelineDrawer_->AddTrack("ポストエフェクト (ヒット時)");

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type       = TrackType::POST_EFFECT_ON_HIT;
            info.trackIndex = trackIdx;
            info.fileName   = param.fileName;
            data_->AddTrackInfo(info);

            int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
            timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + param.fileName);
        }
    }

    // パーティクルエフェクト（複数対応）— リストから直接再構築
    {
        const auto& particleEffectList = renditionData.GetParticleEffectList();
        for (const auto& param : particleEffectList) {
            if (param.fileName.empty() || param.fileName == "None") {
                continue;
            }

            int32_t trackIdx = timelineDrawer_->AddTrack("パーティクルエフェクト");

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type       = TrackType::PARTICLE_EFFECT;
            info.trackIndex = trackIdx;
            info.fileName   = param.fileName;
            data_->AddTrackInfo(info);

            int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
            timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + param.fileName);
        }

        const auto& particleEffectOnHitList = renditionData.GetParticleEffectOnHitList();
        for (const auto& param : particleEffectOnHitList) {
            if (param.fileName.empty() || param.fileName == "None") {
                continue;
            }

            int32_t trackIdx = timelineDrawer_->AddTrack("パーティクルエフェクト (ヒット時)");

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type       = TrackType::PARTICLE_EFFECT_ON_HIT;
            info.trackIndex = trackIdx;
            info.fileName   = param.fileName;
            data_->AddTrackInfo(info);

            int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
            timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + param.fileName);
        }
    }

    // ヒット時演出
    for (const auto& m : kOnHitMapping) {
        int32_t rendIdx       = static_cast<int32_t>(m.rendType);
        const auto& param     = renditionData.GetRenditionParamOnHitFromIndex(rendIdx);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = std::string(PlayerAttackRenditionData::kRenditionTypeInfos[rendIdx].label) + " (ヒット時)";
        int32_t trackIdx      = timelineDrawer_->AddTrack(trackName);

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type            = m.trackType;
        info.trackIndex      = trackIdx;
        info.fileName        = param.fileName;
        info.isCameraReset   = param.isCameraReset;
        info.volume          = param.volume;
        info.repeatOnDamage  = param.repeatOnDamage;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + param.fileName);
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupObjectAnimationTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_)
        return;

    auto& renditionData = attackData_->GetRenditionDataForPhase(phase_);

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& param = renditionData.GetObjAnimationParamFromIndex(i);

        bool isMainHead = (i == static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::MainHead));

        if (param.fileName.empty() || param.fileName == "None") {
            // MainHeadの場合、アニメファイルなしでもトレイルだけ設定されていればトレイルトラックを作成
            if (isMainHead && !param.trailFileName.empty() && param.trailFileName != "None") {
                int32_t trailTrackIdx = timelineDrawer_->AddTrack("MainHead追従トレイル");

                PlayerComboAttackTimelineData::TrackInfo trailInfo;
                trailInfo.type       = PlayerComboAttackTimelineData::TrackType::RIBBON_TRAIL_MAIN_HEAD;
                trailInfo.trackIndex = trailTrackIdx;
                trailInfo.fileName   = param.trailFileName;
                data_->AddTrackInfo(trailInfo);

                timelineDrawer_->AddKeyFrame(trailTrackIdx, 0, 1.0f, 1.0f, "トレイル:" + param.trailFileName);
            }
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kObjAnimationTypeInfos[i].label;
        int32_t trackIdx      = timelineDrawer_->AddTrack(trackName);

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type = static_cast<PlayerComboAttackTimelineData::TrackType>(
            static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD) + i);
        info.trackIndex    = trackIdx;
        info.fileName      = param.fileName;
        info.trailFileName = param.trailFileName;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f,
            "使用ファイル:" + param.fileName);

        // MainHeadかつトレイルも設定済みなら独立したトレイルトラックを追加
        if (isMainHead && !param.trailFileName.empty() && param.trailFileName != "None") {
            int32_t trailTrackIdx = timelineDrawer_->AddTrack("MainHead追従トレイル");

            PlayerComboAttackTimelineData::TrackInfo trailInfo;
            trailInfo.type       = PlayerComboAttackTimelineData::TrackType::RIBBON_TRAIL_MAIN_HEAD;
            trailInfo.trackIndex = trailTrackIdx;
            trailInfo.fileName   = param.trailFileName;
            data_->AddTrackInfo(trailInfo);

            timelineDrawer_->AddKeyFrame(trailTrackIdx, 0, 1.0f, 1.0f, "トレイル:" + param.trailFileName);
        }
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupAudioTracks() {

}

void PlayerComboAttackTimelineTrackBuilder::SetupVibrationTrack() {
    if (!attackData_ || !timelineDrawer_ || !data_)
        return;

    const auto& vibParam = attackData_->GetRenditionDataForPhase(phase_).GetVibrationParam();

    if (vibParam.intensity <= 0.0f)
        return;

    int32_t trackIdx = timelineDrawer_->AddTrack("コントローラ振動");

    PlayerComboAttackTimelineData::TrackInfo info;
    info.type                = PlayerComboAttackTimelineData::TrackType::VIBRATION;
    info.trackIndex          = trackIdx;
    info.vibrationIntensity = vibParam.intensity;
    info.triggerByHit       = vibParam.triggerByHit;
    info.repeatOnDamage     = vibParam.repeatOnDamage;
    data_->AddTrackInfo(info);

    int32_t startFrame    = KetaEngine::Frame::TimeToFrame(vibParam.startTiming);
    float durationFrames  = static_cast<float>(KetaEngine::Frame::TimeToFrame(vibParam.duration));
    if (durationFrames < 1.0f) durationFrames = 1.0f;

    timelineDrawer_->AddKeyFrame(trackIdx, startFrame, vibParam.intensity, durationFrames,
        "振動強度:" + std::to_string(vibParam.intensity));
}

void PlayerComboAttackTimelineTrackBuilder::RebuildBranchTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_) {
        return;
    }

    if (phase_ != AttackTimelinePhase::MAIN) {
        return;
    }

    // 既存の分岐トラックを削除
    std::vector<int32_t> trackIndicesToRemove;
    const auto& addedTracks = data_->GetAddedTracks();
    for (const auto& trackInfo : addedTracks) {
        if (trackInfo.type == PlayerComboAttackTimelineData::TrackType::CANCEL_TIME ||
            trackInfo.type == PlayerComboAttackTimelineData::TrackType::PRECEDE_INPUT) {
            trackIndicesToRemove.push_back(trackInfo.trackIndex);
        }
    }

    // インデックスを降順でソートして後ろから削除
    std::sort(trackIndicesToRemove.begin(), trackIndicesToRemove.end(), std::greater<int32_t>());

    // 削除前に有効なインデックスかチェック
    size_t trackCount = timelineDrawer_->GetTrackCount();
    for (int32_t trackIdx : trackIndicesToRemove) {
        // 範囲外チェック
        if (trackIdx < 0 || trackIdx >= static_cast<int32_t>(trackCount)) {
            continue;
        }
        data_->RemoveTrackInfo(trackIdx);
        timelineDrawer_->RemoveTrack(trackIdx);
        trackCount--; // 削除後のトラック数を更新
    }

    // 挿入位置を計算
    int32_t finishWaitTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT);

    // finishWaitTrackIdxが有効か確認
    if (finishWaitTrackIdx < 0) {
        return;
    }

    int32_t insertPosition = finishWaitTrackIdx + 1;

    // 新しい分岐トラックを追加
    const auto& branches = attackData_->GetComboBranches();

    // 分岐がない場合は早期リターン
    if (branches.empty()) {
        return;
    }

    int32_t totalFrames = CalculateTotalFrames();

    // totalFramesが0以下の場合は最小値を設定
    if (totalFrames <= 0) {
        totalFrames = 1;
    }

    int32_t insertedCount = 0;
    for (size_t i = 0; i < branches.size(); ++i) {
        const auto& branch = branches[i];
        if (!branch) {
            continue; // nullチェック
        }

        std::string buttonName = GetButtonDisplayName(branch->GetKeyboardButton(), branch->GetGamepadButton());

        // キャンセル開始トラック
        {
            std::string trackName = "キャンセル開始（" + buttonName + "）";
            int32_t trackIdx = timelineDrawer_->InsertTrack(insertPosition, trackName);

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type = PlayerComboAttackTimelineData::TrackType::CANCEL_TIME;
            info.trackIndex = trackIdx;
            info.branchIndex = static_cast<int32_t>(i);
            data_->AddTrackInfo(info);

            int32_t cancelFrame = KetaEngine::Frame::TimeToFrame(branch->GetCancelTime());
            // durationが負にならないように
            float duration = static_cast<float>((std::max)(0, totalFrames - cancelFrame));
            timelineDrawer_->AddKeyFrame(trackIdx, cancelFrame, 1.0f, duration, "キャンセル可能範囲");

            insertPosition++;
            insertedCount++;
        }

        // 先行入力開始トラック
        {
            std::string trackName = "先行入力開始（" + buttonName + "）";
            int32_t trackIdx = timelineDrawer_->InsertTrack(insertPosition, trackName);

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type = PlayerComboAttackTimelineData::TrackType::PRECEDE_INPUT;
            info.trackIndex = trackIdx;
            info.branchIndex = static_cast<int32_t>(i);
            data_->AddTrackInfo(info);

            int32_t precedeFrame = KetaEngine::Frame::TimeToFrame(branch->GetPrecedeInputTime());
            // durationが負にならないように
            float duration = static_cast<float>((std::max)(0, totalFrames - precedeFrame));
            timelineDrawer_->AddKeyFrame(trackIdx, precedeFrame, 1.0f, duration, "先行入力可能範囲");

            insertPosition++;
            insertedCount++;
        }
    }

    // トラックインデックスの更新（挿入によりずれた分を調整）
    if (insertedCount > 0) {
        data_->UpdateTrackIndicesAfterInsert(finishWaitTrackIdx + 1, insertedCount);
    }
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

    if (type == PlayerComboAttackTimelineData::TrackType::VIBRATION) {
        info.vibrationIntensity = 0.5f;
        info.triggerByHit       = false;
        data_->AddTrackInfo(info);
        timelineDrawer_->AddKeyFrame(trackIdx, 0, 0.5f, 6.0f, "振動強度:0.5");
        return;
    }

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

    auto& attackParam = attackData_->GetAttackParamForPhase(phase_);

    float totalTime = attackParam.moveParam.startTime + attackParam.moveParam.easeTime + attackParam.moveParam.finishTimeOffset + attackParam.timingParam.finishWaitTime;

    return KetaEngine::Frame::TimeToFrame(totalTime);
}



// ボタン名取得用ヘルパー
std::string PlayerComboAttackTimelineTrackBuilder::GetButtonDisplayName(int32_t keyboardButton, int32_t gamepadButton) {
    std::string name;

    // キーボードボタン名
    if (keyboardButton > 0) {
        name = GetKeyboardKeyName(keyboardButton);
    }

    // ゲームパッドボタン名も追加
    if (gamepadButton > 0) {
        if (!name.empty()) {
            name += "/";
        }
        name += GetGamepadButtonName(gamepadButton);
    }

    if (name.empty()) {
        name = "未設定";
    }

    return name;
}