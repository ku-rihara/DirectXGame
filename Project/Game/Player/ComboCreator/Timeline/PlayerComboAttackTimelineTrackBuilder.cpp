#include <format>
#include "PlayerComboAttackTimelineTrackBuilder.h"
#include "../ComboBranchParameter.h"
#include "../PlayerAttackRenditionData.h"
#include "../PlayerComboAttackData.h"
#include "Frame/Frame.h"
#include "Input/InputData.h"
#include <algorithm>


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
    if (!attackData_ || !timelineDrawer_ || !data_) {
        return;
    }

    auto& attackParam = attackData_->GetAttackParamForPhase(phase_);

    // 総フレーム数を計算して設定
    int32_t totalFrames = CalculateTotalFrames();
    timelineDrawer_->SetEndFrame(totalFrames);

    // コライダートラック
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
                static_cast<float>(durationFrames), "コライダー適用時間");

            // 先頭以外のキーフレームは位置をロック
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

    // 終了処理がある場合はFINISHフェーズ、ない場合はMAINフェーズに分岐トラックを配置
    bool shouldAddBranchTracks =
        (phase_ == AttackTimelinePhase::MAIN   && !attackData_->HasFinishPhase()) ||
        (phase_ == AttackTimelinePhase::FINISH);
    if (!shouldAddBranchTracks) {
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

    // RenditionData::Type → TrackType の正引きマッピング
    struct RendToTrack { RendType rendType; TrackType trackType; TrackType trackTypeOnHit; };
    static constexpr RendToTrack kMapping[] = {
        {RendType::CameraAction,   TrackType::CAMERA_ACTION,   TrackType::CAMERA_ACTION_ON_HIT},
        {RendType::HitStop,        TrackType::HIT_STOP,        TrackType::HIT_STOP_ON_HIT},
        {RendType::ShakeAction,    TrackType::SHAKE_ACTION,    TrackType::SHAKE_ACTION_ON_HIT},
        {RendType::PostEffect,     TrackType::POST_EFFECT,     TrackType::POST_EFFECT_ON_HIT},
        {RendType::ParticleEffect, TrackType::PARTICLE_EFFECT, TrackType::PARTICLE_EFFECT_ON_HIT},
        {RendType::AudioAttack,    TrackType::AUDIO_ATTACK,    TrackType::AUDIO_ATTACK_ON_HIT},
    };

   // 同一fileName区間を1トラックにまとめる
    auto buildTracks = [this](const std::vector<PlayerAttackRenditionData::RenditionParam>& list,
                               TrackType trackType, const std::string& trackName, bool isOnHit) {
        size_t i = 0;
        while (i < list.size()) {
            const auto& first = list[i];
            if (first.fileName.empty() || first.fileName == "None") {
                ++i;
                continue;
            }

            int32_t trackIdx = timelineDrawer_->AddTrack(trackName);

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type           = trackType;
            info.trackIndex     = trackIdx;
            info.fileName       = first.fileName;
            info.isCameraReset  = first.isCameraReset;
            info.volume         = first.volume;
            info.repeatOnDamage = isOnHit ? first.repeatOnDamage : false;
            data_->AddTrackInfo(info);

            size_t j = i;
            while (j < list.size() && list[j].fileName == first.fileName) {
                int32_t frame = KetaEngine::Frame::TimeToFrame(list[j].startTiming);
                timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + first.fileName);
                ++j;
            }
            i = j;
        }
    };

    for (const auto& m : kMapping) {
        int32_t rendIdx    = static_cast<int32_t>(m.rendType);
        std::string label  = PlayerAttackRenditionData::kRenditionTypeInfos[rendIdx].label;

        buildTracks(renditionData.GetRenditionListFromType(m.rendType), m.trackType, label, false);
        buildTracks(renditionData.GetRenditionOnHitListFromType(m.rendType), m.trackTypeOnHit, label + " (ヒット時)", true);
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupObjectAnimationTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_){
        return;
    }

    auto& renditionData = attackData_->GetRenditionDataForPhase(phase_);

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& list = renditionData.GetObjAnimationListFromIndex(i);
        bool isMainHead  = (i == static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::MainHead));

        std::string trackName = PlayerAttackRenditionData::kObjAnimationTypeInfos[i].label;
        auto trackType = static_cast<PlayerComboAttackTimelineData::TrackType>(
            static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD) + i);

        // 同一fileNameが連続するエントリを1トラックの複数キーフレームとしてグルーピングして復元する
        size_t idx = 0;
        while (idx < list.size()) {
            const auto& first = list[idx];
            if (first.fileName.empty() || first.fileName == "None") {
                ++idx;
                continue;
            }

            int32_t trackIdx = timelineDrawer_->AddTrack(trackName);

            PlayerComboAttackTimelineData::TrackInfo info;
            info.type          = trackType;
            info.trackIndex    = trackIdx;
            info.fileName      = first.fileName;
            info.trailFileName = first.trailFileName;
            data_->AddTrackInfo(info);

            size_t j = idx;
            while (j < list.size() && list[j].fileName == first.fileName) {
                int32_t frame = KetaEngine::Frame::TimeToFrame(list[j].startTiming);
                timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + first.fileName);
                ++j;
            }
            idx = j;
        }

        // MainHead追従トレイル: トレイルファイルが設定されている最初のエントリから1トラックだけ復元
        if (isMainHead) {
            for (const auto& param : list) {
                if (!param.trailFileName.empty() && param.trailFileName != "None") {
                    int32_t trailTrackIdx = timelineDrawer_->AddTrack("MainHead追従トレイル");

                    PlayerComboAttackTimelineData::TrackInfo trailInfo;
                    trailInfo.type       = PlayerComboAttackTimelineData::TrackType::RIBBON_TRAIL_MAIN_HEAD;
                    trailInfo.trackIndex = trailTrackIdx;
                    trailInfo.fileName   = param.trailFileName;
                    data_->AddTrackInfo(trailInfo);

                    timelineDrawer_->AddKeyFrame(trailTrackIdx, 0, 1.0f, 1.0f, "トレイル:" + param.trailFileName);
                    break;
                }
            }
        }
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupAudioTracks() {

}

void PlayerComboAttackTimelineTrackBuilder::SetupVibrationTrack() {
    if (!attackData_ || !timelineDrawer_ || !data_)
        return;

    const auto& list = attackData_->GetRenditionDataForPhase(phase_).GetVibrationList();

    // 強度/トリガー条件が同じ連続エントリを1トラックの複数キーフレームとしてグルーピングして復元する
    size_t idx = 0;
    while (idx < list.size()) {
        const auto& first = list[idx];
        if (first.intensity <= 0.0f) {
            ++idx;
            continue;
        }

        int32_t trackIdx = timelineDrawer_->AddTrack("コントローラ振動");

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type               = PlayerComboAttackTimelineData::TrackType::VIBRATION;
        info.trackIndex         = trackIdx;
        info.vibrationIntensity = first.intensity;
        info.triggerByHit       = first.triggerByHit;
        info.repeatOnDamage     = first.repeatOnDamage;
        data_->AddTrackInfo(info);

        size_t j = idx;
        while (j < list.size() &&
               list[j].intensity == first.intensity &&
               list[j].triggerByHit == first.triggerByHit &&
               list[j].repeatOnDamage == first.repeatOnDamage) {
            int32_t startFrame   = KetaEngine::Frame::TimeToFrame(list[j].startTiming);
            float durationFrames = static_cast<float>(KetaEngine::Frame::TimeToFrame(list[j].duration));
            if (durationFrames < 1.0f) durationFrames = 1.0f;

            timelineDrawer_->AddKeyFrame(trackIdx, startFrame, list[j].intensity, durationFrames,
                std::format("振動強度:{}", list[j].intensity));
            ++j;
        }
        idx = j;
    }
}

void PlayerComboAttackTimelineTrackBuilder::RebuildBranchTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_) {
        return;
    }

    // 終了処理がある場合はFINISHフェーズ、ない場合はMAINフェーズに分岐トラックを配置
    bool isTargetPhase =
        (phase_ == AttackTimelinePhase::MAIN   && !attackData_->HasFinishPhase()) ||
        (phase_ == AttackTimelinePhase::FINISH);
    if (!isTargetPhase) {
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

void PlayerComboAttackTimelineTrackBuilder::AddKeyFrameToTrack(int32_t trackIndex) {
    if (!timelineDrawer_ || !data_) {
        return;
    }

    if (trackIndex < 0 || trackIndex >= static_cast<int32_t>(timelineDrawer_->GetTrackCount())) {
        return;
    }

    auto* trackInfo = data_->FindTrackInfo(trackIndex);

    // 直前のキーフレームの終端の後ろに新しいキーフレームを追加
    const auto& keyframes = timelineDrawer_->GetTracks()[trackIndex].keyframes;
    int32_t newFrame = 0;
    for (const auto& kf : keyframes) {
        int32_t endFrame = kf.frame + static_cast<int32_t>(kf.duration);
        if (endFrame > newFrame) {
            newFrame = endFrame;
        }
    }

    if (trackInfo && trackInfo->type == PlayerComboAttackTimelineData::TrackType::VIBRATION) {
        timelineDrawer_->AddKeyFrame(trackIndex, newFrame, trackInfo->vibrationIntensity, 6.0f,
            std::format("振動強度:{}", trackInfo->vibrationIntensity));
        return;
    }

    std::string label = (trackInfo && !trackInfo->fileName.empty())
        ? "使用ファイル:" + trackInfo->fileName
        : "ファイル未選択";
    timelineDrawer_->AddKeyFrame(trackIndex, newFrame, 1.0f, 1.0f, label);
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