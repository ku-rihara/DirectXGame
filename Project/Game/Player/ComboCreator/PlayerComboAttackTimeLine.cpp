#include "PlayerComboAttackTimeline.h"
#include <imgui.h>

void PlayerComboAttackTimeline::Init(PlayerComboAttackData* attackData) {
    attackData_ = attackData;

    if (!attackData_) {
        return;
    }

    timeline_.Init();

    // 総フレーム数を設定
    int32_t totalFrames = CalculateTotalFrames();
    timeline_.SetEndFrame(totalFrames);

    // トラックインデックスを初期化
    defaultTrackIndices_.fill(-1);
    renditionTrackIndices_.clear();
    objAnimTrackIndices_.clear();
    audioTrackIndices_.clear();

    // デフォルトトラックのセットアップ
    SetupDefaultTracks();

    // 演出トラックのセットアップ
    SetupRenditionTracks();

    // オブジェクトアニメーショントラックのセットアップ
    SetupObjectAnimationTracks();

    // オーディオトラックのセットアップ
    SetupAudioTracks();

    isInitialized_ = true;
}

void PlayerComboAttackTimeline::Draw() {
    if (!isInitialized_ || !attackData_) {
        ImGui::Text("Timeline not initialized");
        return;
    }

    ImGui::PushID("AttackTimeline");

    // 再生モード選択
    ImGui::Text("Play Mode:");
    ImGui::SameLine();
    if (ImGui::RadioButton("Single", playMode_ == PlayMode::SINGLE)) {
        playMode_ = PlayMode::SINGLE;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Continuous", playMode_ == PlayMode::CONTINUOUS)) {
        playMode_ = PlayMode::CONTINUOUS;
    }

    // タイムライン描画
    timeline_.Draw();

    // 適用ボタン
    if (ImGui::Button("Apply to Parameters")) {
        ApplyToParameters();
    }
    ImGui::SameLine();

    // リセットボタン
    if (ImGui::Button("Reset from Parameters")) {
        Init(attackData_);
    }

    ImGui::PopID();

    // 連続再生モードで再生終了時
    if (playMode_ == PlayMode::CONTINUOUS && timeline_.IsPlaying() && timeline_.GetCurrentFrame() >= timeline_.GetEndFrame()) {
        AdvanceToNextAttack();
    }
}

void PlayerComboAttackTimeline::ApplyToParameters() {
    if (!attackData_) {
        return;
    }

    auto& attackParam = attackData_->GetAttackParam();

    // 移動イージング時間
    int32_t moveTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::MOVE_EASING)];
    if (moveTrackIdx >= 0) {
        // 最後のキーフレームの時間を取得して適用
        // この実装は簡略化のため、実際にはキーフレームリストから取得する必要があります
        // ここでは概念的な実装を示します
    }

    // キャンセル開始時間
    int32_t cancelTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)];
    if (cancelTrackIdx >= 0) {
        float cancelTime                   = timeline_.GetValueAtFrame(cancelTrackIdx, timeline_.GetCurrentFrame());
        attackParam.timingParam.cancelTime = FrameToTime(static_cast<int32_t>(cancelTime));
    }

    // 先行入力開始時間
    int32_t precedeTrackIdx = defaultTrackIndices_[static_cast<size_t>(DefaultTrack::PRECEDE_INPUT_START)];
    if (precedeTrackIdx >= 0) {
        float precedeTime                        = timeline_.GetValueAtFrame(precedeTrackIdx, timeline_.GetCurrentFrame());
        attackParam.timingParam.precedeInputTime = FrameToTime(static_cast<int32_t>(precedeTime));
    }
}

void PlayerComboAttackTimeline::SetupDefaultTracks() {
    auto& attackParam = attackData_->GetAttackParam();

    // 移動イージングトラック
    {
        int32_t trackIdx                                                     = timeline_.AddTrack("Move Easing");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::MOVE_EASING)] = trackIdx;

        // 開始位置(フレーム0)
        timeline_.AddKeyFrame(trackIdx, 0, 0.0f);

        // 終了位置(イージング時間)
        int32_t endFrame = TimeToFrame(attackParam.moveParam.easeTime);
        timeline_.AddKeyFrame(trackIdx, endFrame, 1.0f);
    }

    // キャンセル開始トラック
    if (attackParam.timingParam.isCancel) {
        int32_t trackIdx                                                      = timeline_.AddTrack("Cancel Start");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::CANCEL_START)] = trackIdx;

        int32_t cancelFrame = TimeToFrame(attackParam.timingParam.cancelTime);
        timeline_.AddKeyFrame(trackIdx, cancelFrame, 1.0f, 1.0f);
    }

    // 先行入力開始トラック
    {
        int32_t trackIdx                                                             = timeline_.AddTrack("Precede Input Start");
        defaultTrackIndices_[static_cast<size_t>(DefaultTrack::PRECEDE_INPUT_START)] = trackIdx;

        int32_t precedeFrame = TimeToFrame(attackParam.timingParam.precedeInputTime);
        timeline_.AddKeyFrame(trackIdx, precedeFrame, 1.0f, 1.0f);
    }
}

void PlayerComboAttackTimeline::SetupRenditionTracks() {
    auto& renditionData = attackData_->GetRenditionData();

    // 通常演出
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        // トラック名を取得
        std::string trackName = PlayerAttackRenditionData::kRenditionTypeInfos[i].label;

        int32_t trackIdx = timeline_.AddTrack(trackName);
        renditionTrackIndices_.push_back(trackIdx);

        // キーフレーム追加
        int32_t frame = TimeToFrame(param.startTiming);
        timeline_.AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);
    }

    // ヒット時演出
    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::Type::Count); ++i) {
        const auto& param = renditionData.GetRenditionParamOnHitFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = std::string(PlayerAttackRenditionData::kRenditionTypeInfos[i].label) + " (On Hit)";

        int32_t trackIdx = timeline_.AddTrack(trackName);
        renditionTrackIndices_.push_back(trackIdx);

        int32_t frame = TimeToFrame(param.startTiming);
        timeline_.AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);
    }
}

void PlayerComboAttackTimeline::SetupObjectAnimationTracks() {
    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::ObjAnimationType::Count); ++i) {
        const auto& param = renditionData.GetObjAnimationParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kObjAnimationTypeInfos[i].label;

        int32_t trackIdx = timeline_.AddTrack(trackName);
        objAnimTrackIndices_.push_back(trackIdx);

        int32_t frame = TimeToFrame(param.startTiming);
        timeline_.AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);
    }
}

void PlayerComboAttackTimeline::SetupAudioTracks() {
    auto& renditionData = attackData_->GetRenditionData();

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerAttackRenditionData::AudioType::Count); ++i) {
        const auto& param = renditionData.GetAudioParamFromIndex(i);

        if (param.fileName.empty() || param.fileName == "None") {
            continue;
        }

        std::string trackName = PlayerAttackRenditionData::kAudioTypeInfos[i].label;

        int32_t trackIdx = timeline_.AddTrack(trackName);
        audioTrackIndices_.push_back(trackIdx);

        int32_t frame = TimeToFrame(param.startTiming);
        timeline_.AddKeyFrame(trackIdx, frame, 1.0f, 1.0f);
    }
}

int32_t PlayerComboAttackTimeline::CalculateTotalFrames() const {
    if (!attackData_) {
        return 300;
    }

    auto& attackParam = attackData_->GetAttackParam();

    // イージング時間 + 終了待機時間 + 移動完了時間オフセット
    float totalTime = attackParam.moveParam.easeTime + attackParam.timingParam.finishWaitTime + attackParam.moveParam.finishTimeOffset;

    return TimeToFrame(totalTime) + 60; // 余裕を持たせる
}

void PlayerComboAttackTimeline::AdvanceToNextAttack() {
    // この機能は PlayerComboAttackController と連携して実装する必要があります
    // ここでは概念的な実装のみ示します

    if (!attackData_) {
        return;
    }

    std::string nextAttackName = attackData_->GetAttackParam().nextAttackType;

    if (nextAttackName.empty() || nextAttackName == "None") {
        timeline_.SetPlaying(false);
        timeline_.SetCurrentFrame(0);
        return;
    }

    // 次の攻撃への遷移処理
    // 実際の実装では PlayerComboAttackController を通じて次の攻撃を取得し、
    // その攻撃のタイムラインを初期化して再生する必要があります
}