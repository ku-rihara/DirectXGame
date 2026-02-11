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

    // totalFramesが0以下の場合は最小値を設定
    if (totalFrames <= 0) {
        totalFrames = 1;
    }

    // コンボ分岐ごとのトラックを作成
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
            // durationが負にならないように安全チェック
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
            // durationが負にならないように安全チェック
            float duration = static_cast<float>((std::max)(0, totalFrames - precedeFrame));
            timelineDrawer_->AddKeyFrame(trackIdx, precedeFrame, 1.0f, duration, "先行入力可能範囲");
        }
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupRenditionTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_) {
        return;
    }

    // 演出データを取得
    auto& renditionData = attackData_->GetRenditionData();

    // 通常演出
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        // トラック名を取得
        std::string trackName = PlayerAttackRenditionData::kRenditionTypeInfos[i].label;
        // タイムラインに追加したトラックindexを取得
        int32_t trackIdx = timelineDrawer_->AddTrack(trackName);

        // トラックの情報をセット
        PlayerComboAttackTimelineData::TrackInfo info;
        info.type          = static_cast<PlayerComboAttackTimelineData::TrackType>(i);
        info.trackIndex    = trackIdx;
        info.fileName      = param.fileName;
        info.isCameraReset = param.isCameraReset;
        info.volume        = param.volume;
        // トラックに追加
        data_->AddTrackInfo(info);

        // 開始秒数をフレーム換算してキーフレームを配置
        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f, "使用ファイル:" + param.fileName);
    }

    // ヒット時演出
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamOnHitFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = std::string(PlayerAttackRenditionData::kRenditionTypeInfos[i].label) + " (ヒット時)";
        int32_t trackIdx      = timelineDrawer_->AddTrack(trackName);

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type = static_cast<PlayerComboAttackTimelineData::TrackType>(
            static_cast<int>(PlayerComboAttackTimelineData::TrackType::CAMERA_ACTION_ON_HIT) + i);
        info.trackIndex    = trackIdx;
        info.fileName      = param.fileName;
        info.isCameraReset = param.isCameraReset;
        info.volume        = param.volume;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f,
            "使用ファイル:" + param.fileName);
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupObjectAnimationTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_)
        return;

    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& param = renditionData.GetObjAnimationParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kObjAnimationTypeInfos[i].label;
        int32_t trackIdx      = timelineDrawer_->AddTrack(trackName);

        PlayerComboAttackTimelineData::TrackInfo info;
        info.type = static_cast<PlayerComboAttackTimelineData::TrackType>(
            static_cast<int>(PlayerComboAttackTimelineData::TrackType::OBJ_ANIM_HEAD) + i);
        info.trackIndex = trackIdx;
        info.fileName   = param.fileName;
        data_->AddTrackInfo(info);

        int32_t frame = KetaEngine::Frame::TimeToFrame(param.startTiming);
        timelineDrawer_->AddKeyFrame(trackIdx, frame, 1.0f, 1.0f,
            "使用ファイル:" + param.fileName);
    }
}

void PlayerComboAttackTimelineTrackBuilder::SetupAudioTracks() {
    // オーディオはSetupRenditionTracksで処理されるため、この関数は空にする
    // 後方互換性のために残しておく
}

void PlayerComboAttackTimelineTrackBuilder::RebuildBranchTracks() {
    if (!attackData_ || !timelineDrawer_ || !data_) {
        return;
    }

    // 既存の分岐トラックを削除（後ろから削除して index のずれを防ぐ）
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

    // 挿入位置を計算（終了待機時間トラックの直後）
    int32_t finishWaitTrackIdx = data_->GetDefaultTrackIndex(
        PlayerComboAttackTimelineData::DefaultTrack::FINISH_WAIT);

    // 安全チェック：finishWaitTrackIdxが有効か確認
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
            // durationが負にならないように安全チェック
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
            // durationが負にならないように安全チェック
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