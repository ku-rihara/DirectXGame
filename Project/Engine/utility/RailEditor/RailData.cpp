#include "RailData.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <Windows.h>

void RailData::Init(const std::string& railName) {
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = railName;

    // Rail初期化
    rail_ = std::make_unique<Rail>();
    rail_->Init(5);

    if (!globalParameter_->HasRegisters(railName)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_, true);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        // 値取得
        LoadParams();
    }

    // リセット
    ResetParams();
}

void RailData::Update(const float& speed, const PositionMode& mode, const Vector3& direction) {
    // 戻り中の更新処理
    if (playState_ == PlayState::RETURNING) {
        UpdateReturn(speed);
        return;
    }

    if (playState_ != PlayState::PLAYING) {
        return;
    }

    direction_ = direction;

    // Easingを更新
    timeEase_.Update(speed);

    // 開始時間に達していない場合は待機
    if (elapsedTime_ < startTime_) {
        currentPosition_ = startPosition_; // 開始位置を保持
        return;
    }

    // キーフレームから制御点リストを作成
    std::vector<Vector3> positions;
    for (const auto& keyFrame : controlPoints_) {
        Vector3 pos = keyFrame->GetPosition() * direction_;

        if (mode == PositionMode::WORLD && parentTransform_ != nullptr) {
            // ワールド座標に変換
            Matrix4x4 parentMatrix = parentTransform_->matWorld_;
            Vector3 worldPos       = TransformMatrix(pos, parentMatrix);
            positions.push_back(worldPos);
        } else {
            positions.push_back(pos);
        }
    }

    // レール更新
    if (!positions.empty()) {
        rail_->Update(positions);
    }

    LoopOrStop();

    // Easing適用後の時間で現在位置を取得
    currentPosition_ = startPosition_ + rail_->GetPositionOnRail(easedTime_);
}

void RailData::LoopOrStop() {
    // イージングが完了したかチェック
    if (!timeEase_.IsFinished()) {
        return;
    }

    if (isLoop_) {
        timeEase_.Reset();
        easedTime_ = 0.0f;
    } else {
        easedTime_ = 1.0f;

        // 戻りモードが設定されている場合、戻り動作を開始
        if (returnParam_.mode != ReturnMode::NONE) {
            StartReturn();
        } else {
            playState_ = PlayState::STOPPED;
        }
    }
}

void RailData::StartReturn() {
    playState_                = PlayState::RETURNING;
    returnParam_.easeAdaptPos = Vector3::ZeroVector();

    if (returnParam_.mode == ReturnMode::DIRECT_RETURN) {
        // 直接戻る用のイージング設定
        returnParam_.ease.SetAdaptValue(&returnParam_.easeAdaptPos);
        returnParam_.ease.SetStartValue(currentPosition_);
        returnParam_.ease.SetEndValue(startPosition_);
        returnParam_.ease.SetMaxTime(returnParam_.maxTime);
        returnParam_.ease.SetType(static_cast<EasingType>(returnParam_.easeType));
        returnParam_.ease.Reset();
    } else if (returnParam_.mode == ReturnMode::REVERSE_RAIL) {
        // レール逆走用のイージング設定を適用
        timeEase_.SetStartValue(1.0f);
        timeEase_.SetEndValue(0.0f);
        timeEase_.SetMaxTime(returnParam_.maxTime);
        timeEase_.SetType(static_cast<EasingType>(returnParam_.easeType));
    }
}

void RailData::UpdateReturn(const float& speed) {
    switch (returnParam_.mode) {
    case ReturnMode::REVERSE_RAIL:
        // レールを逆走
        timeEase_.Update(speed);

        // easedTimeが0以下になったら終了
        if (easedTime_ <= 0.0f) {
            easedTime_       = 0.0f;
            currentPosition_ = startPosition_;
            playState_       = PlayState::STOPPED;
            // 元のイージング設定に戻す
            timeEase_.SetMaxTime(maxTime_);
            timeEase_.SetType(static_cast<EasingType>(easeType_));
        } else {
            currentPosition_ = startPosition_ + rail_->GetPositionOnRail(easedTime_);
        }
        break;

    case ReturnMode::DIRECT_RETURN:
        // 直接戻る
        returnParam_.ease.Update(speed);

        currentPosition_ = returnParam_.easeAdaptPos;

        if (returnParam_.ease.IsFinished()) {
            currentPosition_ = startPosition_;
            playState_       = PlayState::STOPPED;
        }
        break;

    default:
        break;
    }
}

void RailData::Play() {
    Reset();
    startPosition_ = currentPosition_; // 開始位置を記憶
    playState_     = PlayState::PLAYING;
    timeEase_.Reset();
}

void RailData::Stop() {
    playState_       = PlayState::STOPPED;
    currentPosition_ = startPosition_;
}

void RailData::Reset() {
    elapsedTime_     = 0.0f;
    easedTime_       = 0.0f;
    currentPosition_ = startPosition_;
    timeEase_.Reset();
}

bool RailData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

bool RailData::IsPlaying() const {
    return playState_ == PlayState::PLAYING || playState_ == PlayState::RETURNING;
}

void RailData::AddKeyFrame() {
    int32_t newIndex = static_cast<int32_t>(controlPoints_.size());
    auto newKeyFrame = std::make_unique<RailControlPoint>();
    newKeyFrame->Init(groupName_, newIndex);

    controlPoints_.push_back(std::move(newKeyFrame));
    selectedKeyFrameIndex_ = newIndex;
}

void RailData::RemoveKeyFrame(const int32_t& index) {
    if (index >= 0 && index < static_cast<int32_t>(controlPoints_.size())) {
        controlPoints_.erase(controlPoints_.begin() + index);

        // 選択インデックスの調整
        if (selectedKeyFrameIndex_ >= index) {
            selectedKeyFrameIndex_--;
            if (selectedKeyFrameIndex_ < 0 && !controlPoints_.empty()) {
                selectedKeyFrameIndex_ = 0;
            }
        }

        // インデックスの再設定
        for (int32_t i = 0; i < static_cast<int32_t>(controlPoints_.size()); ++i) {
            controlPoints_[i]->Init(groupName_, i);
        }
    }
}

void RailData::ClearKeyFrames() {
    controlPoints_.clear();
    selectedKeyFrameIndex_ = -1;
}

void RailData::InitKeyFrames() {
    // すべてのキーフレームを初期化
    for (int32_t i = 0; i < static_cast<int32_t>(controlPoints_.size()); ++i) {
        controlPoints_[i]->Init(groupName_, i);
    }
}

void RailData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);

    // キーフレームデータの読み込み
    LoadKeyFrames();
}

void RailData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);

    // キーフレームデータの保存
    SaveAllKeyFrames();
}

void RailData::SaveAllKeyFrames() {
    // すべてのキーフレームを保存
    for (auto& keyFrame : controlPoints_) {
        keyFrame->SaveData();
    }
}

void RailData::LoadKeyFrames() {
    std::string folderPath     = "Resources/GlobalParameter/RailEditor/ControlPoints/";
    std::string keyFramePrefix = groupName_;

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {

        std::vector<std::pair<int32_t, std::string>> keyFrameFiles;

        // キーフレームファイルを検索
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // ファイル名チェック
                if (fileName.find(keyFramePrefix) == 0) {
                    // インデックス番号を抽出
                    std::string indexStr = fileName.substr(keyFramePrefix.length());

                    int32_t index = std::stoi(indexStr);
                    keyFrameFiles.emplace_back(index, fileName);
                }
            }
        }

        // インデックス順にソート
        std::sort(keyFrameFiles.begin(), keyFrameFiles.end());

        RebuildAndLoadAllKeyFrames(keyFrameFiles);

        // 最初のキーフレームを選択状態に
        if (!controlPoints_.empty()) {
            selectedKeyFrameIndex_ = 0;
        }
    }
}

void RailData::RebuildAndLoadAllKeyFrames(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles) {
    // 再構築
    if (controlPoints_.size() == 0) {
        // 既存のキーフレームをクリア
        ClearKeyFrames();
        // キーフレームを作成してロード
        for (const auto& [index, fileName] : KeyFrameFiles) {
            auto newKeyFrame = std::make_unique<RailControlPoint>();
            newKeyFrame->Init(groupName_, index);
            newKeyFrame->LoadData();
            controlPoints_.push_back(std::move(newKeyFrame));
        }
    } else {
        // すべてのキーフレームをロード
        for (auto& keyFrame : controlPoints_) {
            keyFrame->LoadData();
        }
    }
}

void RailData::RegisterParams() {
    globalParameter_->Regist(groupName_, "maxTime", &maxTime_);
    globalParameter_->Regist(groupName_, "startTime", &startTime_);
    globalParameter_->Regist(groupName_, "isLoop", &isLoop_);
    globalParameter_->Regist(groupName_, "easeType", &easeType_);
    globalParameter_->Regist(groupName_, "returnMode", &returnParam_.modeInt);
    globalParameter_->Regist(groupName_, "returnTime", &returnParam_.maxTime);
    globalParameter_->Regist(groupName_, "returnEaseType", &returnParam_.easeType);
}

void RailData::LoadParams() {
    maxTime_              = globalParameter_->GetValue<float>(groupName_, "maxTime");
    startTime_            = globalParameter_->GetValue<float>(groupName_, "startTime");
    isLoop_               = globalParameter_->GetValue<bool>(groupName_, "isLoop");
    easeType_             = globalParameter_->GetValue<int32_t>(groupName_, "easeType");
    returnParam_.modeInt  = globalParameter_->GetValue<int32_t>(groupName_, "returnMode");
    returnParam_.maxTime  = globalParameter_->GetValue<float>(groupName_, "returnTime");
    returnParam_.easeType = globalParameter_->GetValue<int32_t>(groupName_, "returnEaseType");
    returnParam_.mode     = static_cast<ReturnMode>(returnParam_.modeInt);
}

void RailData::ResetParams() {
    playState_       = PlayState::STOPPED;
    currentPosition_ = Vector3::ZeroVector();
    startPosition_   = Vector3::ZeroVector();
    elapsedTime_     = 0.0f;
    easedTime_       = 0.0f;

    // Easing初期化
    timeEase_.SetAdaptValue(&easedTime_);
    timeEase_.SetStartValue(0.0f);
    timeEase_.SetEndValue(1.0f);
    timeEase_.SetMaxTime(maxTime_);
    timeEase_.SetType(static_cast<EasingType>(easeType_));
}

void RailData::ImGuiKeyFrameList() {
    ImGui::SeparatorText("KeyFrames");

    if (ImGui::Button("Add KeyFrame")) {
        AddKeyFrame();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear All KeyFrames")) {
        ClearKeyFrames();
    }

    ImGui::Separator();

    for (int32_t i = 0; i < static_cast<int32_t>(controlPoints_.size()); ++i) {
        ImGui::PushID(i);
        controlPoints_[i]->AdjustParam();
        ImGui::PopID();
    }
}

void RailData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Rail Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // 再生制御
        if (ImGui::Button("Play")) {
            Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            Stop();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            Reset();
        }

        // 状態表示
        const char* stateText = "";
        switch (playState_) {
        case PlayState::STOPPED:
            stateText = "STOPPED";
            break;
        case PlayState::PLAYING:
            stateText = "PLAYING";
            break;
        case PlayState::PAUSED:
            stateText = "PAUSED";
            break;
        case PlayState::RETURNING:
            stateText = "RETURNING";
            break;
        }
        ImGui::Text("State: %s", stateText);

        // 進行状況を表示
        float progress = easedTime_;
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Progress");

        ImGui::Separator();

        // パラメータ調整
        ImGui::DragFloat("Start Time", &startTime_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Max Time", &maxTime_, 0.01f, 0.1f, 10.0f);
        ImGui::Checkbox("Loop", &isLoop_);

        ImGui::Separator();

        // イージングタイプ選択
        ImGuiEasingTypeSelector("Easing Type", easeType_);

        // パラメータをEasingに反映
        timeEase_.SetMaxTime(maxTime_);
        timeEase_.SetType(static_cast<EasingType>(easeType_));

        ImGui::Separator();

        // 戻りモードの設定
        ImGui::SeparatorText("Return Settings");
        const char* returnModes[] = {"None", "Reverse Rail", "Direct Return"};
        if (ImGui::Combo("Return Mode", &returnParam_.modeInt, returnModes, IM_ARRAYSIZE(returnModes))) {
            returnParam_.mode = static_cast<ReturnMode>(returnParam_.modeInt);
        }

        // 戻り用の設定を表示
        if (returnParam_.mode != ReturnMode::NONE) {
            ImGui::DragFloat("Return Time", &returnParam_.maxTime, 0.01f, 0.1f, 10.0f);
            ImGuiEasingTypeSelector("Return Easing Type", returnParam_.easeType);
        }

        ImGui::Separator();

        // キーフレームリスト
        if (showKeyFrameList_) {
            ImGuiKeyFrameList();
        }

        ImGui::PopID();
    }
#endif // _DEBUG
}

void RailData::SetSelectedKeyFrameIndex(const int32_t& index) {
    if (index >= -1 && index < static_cast<int32_t>(controlPoints_.size())) {
        selectedKeyFrameIndex_ = index;
    }
}