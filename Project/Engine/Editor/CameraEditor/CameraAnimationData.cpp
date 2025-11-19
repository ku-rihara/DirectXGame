#include "CameraAnimationData.h"
#undef min
#undef max
#include "Frame/Frame.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <imgui.h>
#include <iostream>

void CameraAnimationData::Init(const std::string& animationName) {

    // グループ名設定
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = animationName;

    if (!globalParameter_->HasRegisters(animationName)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        // パラメータを取得
        GetParams();
    }

    // 値初期化
    ResetParams();
}

void CameraAnimationData::ResetParams() {
    activeKeyFrameIndex_ = 0;

    // 新しいフラグの初期化
    isAllKeyFramesFinished_         = false;
    lastCompletedKeyFrameIndex_     = -1;
    returnParam_.isWaitingForReturn = false;
    resetParam_.currentDelayTimer   = 0.0f;

    returnParam_.positionEase.SetAdaptValue(&returnCameraTransform_.position);
    returnParam_.rotationEase.SetAdaptValue(&returnCameraTransform_.rotation);
    returnParam_.fovEase.SetAdaptValue(&returnCameraTransform_.fov);
}

void CameraAnimationData::LoadData() {

    // アニメーションデータのロード
    globalParameter_->LoadFile(groupName_, folderPath_);
    // キーフレームデータのロード
    LoadKeyFrames();
    // 値同期
    globalParameter_->SyncParamForGroup(groupName_);
}

void CameraAnimationData::SaveData() {

    // アニメーションデータの保存
    globalParameter_->SaveFile(groupName_, folderPath_);
    // キーフレームデータの保存
    SaveAllKeyFrames();
}

void CameraAnimationData::SaveAllKeyFrames() {
    // すべてのキーフレームを保存
    for (auto& keyFrame : keyFrames_) {
        keyFrame->SaveData();
    }
}

void CameraAnimationData::LoadKeyFrames() {
    std::string folderPath     = keyFrameFolderPath_ + groupName_ + "/";
    std::string keyFramePrefix = groupName_;

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {

        std::vector<std::pair<int32_t, std::string>> keyFrameFiles;

        // キーフレームファイルを検索
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // ファイルチェック
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

        CreateOrLoadKeyFrames(keyFrameFiles);
    }
}

void CameraAnimationData::CreateOrLoadKeyFrames(const std::vector<std::pair<int32_t, std::string>>& KeyFrameFiles) {

    // 作成
    if (keyFrames_.size() == 0) {
        // 既存のキーフレームをクリア
        ClearKeyFrames();
        // キーフレームを作成してロード
        for (const auto& [index, fileName] : KeyFrameFiles) {
            auto newKeyFrame = std::make_unique<CameraKeyFrame>();
            newKeyFrame->Init(groupName_, index);
            newKeyFrame->LoadData(); // Load
            keyFrames_.push_back(std::move(newKeyFrame));
        }
    } else {

        // すべてのキーフレームを保存
        for (auto& keyFrame : keyFrames_) {
            keyFrame->LoadData();
        }
    }
}

void CameraAnimationData::Update(const float& speedRate) {
    // 再生中の更新
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    // キーフレーム進行管理
    UpdateKeyFrameProgression();

    // アクティブなキーフレームのみ更新
    UpdateActiveKeyFrames(speedRate);

    // 補間値の更新
    UpdateInterpolatedValues();
}

void CameraAnimationData::UpdateActiveKeyFrames(const float& speedRate) {
    if (keyFrames_.empty()) {
        return;
    }

    float actualDeltaTime = 0.0f;
    switch (static_cast<CameraKeyFrame::TimeMode>(timeMode_)) {
    case CameraKeyFrame::TimeMode::DELTA_TIME:
        // タイムスケール無視
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case CameraKeyFrame::TimeMode::DELTA_TIME_RATE:
        // タイムスケール適用
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    default:
        break;
    }

    // returnEasing開始待機中の処理
    if (returnParam_.isWaitingForReturn) {
        resetParam_.currentDelayTimer += actualDeltaTime;

        // 待機時間を超えたらイージング開始
        if (resetParam_.currentDelayTimer >= resetParam_.delayTime) {
            returnParam_.isWaitingForReturn   = false;
            returnParam_.isReturningToInitial = true;
            resetParam_.currentDelayTimer     = 0.0f;
        }
        return;
    }

    // 初期値に戻るイージング
    if (returnParam_.isReturningToInitial) {
        returnParam_.positionEase.Update(actualDeltaTime);
        returnParam_.rotationEase.Update(actualDeltaTime);
        returnParam_.fovEase.Update(actualDeltaTime);

        // イージングが完了したかチェック
        if (returnParam_.positionEase.IsFinished() && returnParam_.rotationEase.IsFinished() && returnParam_.fovEase.IsFinished()) {
            returnParam_.isReturningToInitial = false;
            isAllFinished_                    = true;
            playState_                        = PlayState::STOPPED;
        }
        return;
    }

    // 現在のアクティブキーフレームを更新
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        keyFrames_[activeKeyFrameIndex_]->Update(speedRate);
    }
}

void CameraAnimationData::UpdateKeyFrameProgression() {
    if (keyFrames_.empty() || playState_ != PlayState::PLAYING) {
        return;
    }

    // 現在のキーフレームが完了したかチェック
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        if (!keyFrames_[activeKeyFrameIndex_]->IsFinished()) {
            return;
        }

        // 完了したキーフレームのインデックスを記録
        lastCompletedKeyFrameIndex_ = activeKeyFrameIndex_;

        // 最後のキーフレームかチェック
        if (activeKeyFrameIndex_ == static_cast<int32_t>(keyFrames_.size()) - 1) {
            // 最後のキーフレームに到達
            isAllKeyFramesFinished_ = true;

            // 最終キーフレームインデックスを設定
            finalKeyFrameIndex_ = activeKeyFrameIndex_;

            if (returnParam_.autoReturnToInitial) {
                // 初期値復帰を開始
                StartReturnToInitial();
            } else {
                playState_ = PlayState::STOPPED;
            }
        } else {
            // 次のキーフレームに進む
            AdvanceToNextKeyFrame();
        }
    }
}

void CameraAnimationData::AdvanceToNextKeyFrame() {
    if (activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size()) - 1) {
        activeKeyFrameIndex_++;

        // 次のキーフレームを初期化
        if (activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
            // 前のキーフレームの最終値を取得
            Vector3 startPos = currentCameraTransform_.position;
            Vector3 startRot = currentCameraTransform_.rotation;
            float startFov   = currentCameraTransform_.fov;

            keyFrames_[activeKeyFrameIndex_]->SetStartEasing(startPos, startRot, startFov);
        }
    }
}

void CameraAnimationData::UpdateInterpolatedValues() {
    if (keyFrames_.empty()) {
        return;
    }

    // 初期値復帰中の場合
    if (returnParam_.isReturningToInitial) {
        currentCameraTransform_.position = returnCameraTransform_.position;
        currentCameraTransform_.rotation = returnCameraTransform_.rotation;
        currentCameraTransform_.fov      = returnCameraTransform_.fov;
    } else if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        // アクティブなキーフレームから現在の補間値を取得
        currentCameraTransform_.position = keyFrames_[activeKeyFrameIndex_]->GetPosition();
        currentCameraTransform_.rotation = keyFrames_[activeKeyFrameIndex_]->GetRotation();
        currentCameraTransform_.fov      = keyFrames_[activeKeyFrameIndex_]->GetFov();
    }
}

void CameraAnimationData::ApplyToViewProjection(ViewProjection& viewProjection) {

    // viewProjectionの値適応
    if (playState_ == PlayState::PLAYING) {
        viewProjection.positionOffset_ = currentCameraTransform_.position;
        viewProjection.rotationOffset_ = currentCameraTransform_.rotation;
        viewProjection.fovAngleY_      = currentCameraTransform_.fov;
    } else if (isAllFinished_) {
        viewProjection.positionOffset_ = initialCameraTransform_.position;
        viewProjection.rotationOffset_ = initialCameraTransform_.rotation;
        viewProjection.fovAngleY_      = initialCameraTransform_.fov;
    }
}

void CameraAnimationData::AddKeyFrame() {
    int32_t newIndex = static_cast<int32_t>(keyFrames_.size());
    auto newKeyFrame = std::make_unique<CameraKeyFrame>();
    newKeyFrame->Init(groupName_, newIndex);

    keyFrames_.push_back(std::move(newKeyFrame));
    selectedKeyFrameIndex_ = newIndex;

    // フラグをリセット
    isAllKeyFramesFinished_ = false;
}

void CameraAnimationData::RemoveKeyFrame(const int32_t& index) {
    if (index >= 0 && index < static_cast<int32_t>(keyFrames_.size())) {
        keyFrames_.erase(keyFrames_.begin() + index);

        // 選択インデックスの調整
        if (selectedKeyFrameIndex_ >= index) {
            selectedKeyFrameIndex_--;
            if (selectedKeyFrameIndex_ < 0 && !keyFrames_.empty()) {
                selectedKeyFrameIndex_ = 0;
            }
        }

        // アクティブインデックスの調整
        if (activeKeyFrameIndex_ >= index) {
            activeKeyFrameIndex_--;
            if (activeKeyFrameIndex_ < 0 && !keyFrames_.empty()) {
                activeKeyFrameIndex_ = 0;
            }
        }

        // インデックスの再設定
        for (int32_t i = 0; i < static_cast<int32_t>(keyFrames_.size()); ++i) {
            keyFrames_[i]->Init(groupName_, i);
        }

        // フラグをリセット
        isAllKeyFramesFinished_ = false;

        lastCompletedKeyFrameIndex_ = -1;
    }
}

void CameraAnimationData::ClearKeyFrames() {
    keyFrames_.clear();
    selectedKeyFrameIndex_ = -1;
    activeKeyFrameIndex_   = 0;

    // フラグをリセット
    isAllKeyFramesFinished_ = false;

    lastCompletedKeyFrameIndex_ = -1;
}

void CameraAnimationData::InitKeyFrames() {
    // すべてのキーフレームを保存
    for (int32_t i = 0; i < keyFrames_.size(); ++i) {
        keyFrames_[i]->Init(groupName_, i);
    }
}

bool CameraAnimationData::IsFinished() const {
    return isAllKeyFramesFinished_;
}

void CameraAnimationData::Play() {
    Reset();
    playState_ = PlayState::PLAYING;
}

void CameraAnimationData::Pause() {
    if (playState_ == PlayState::PLAYING) {
        playState_ = PlayState::PAUSED;
    } else if (playState_ == PlayState::PAUSED) {
        playState_ = PlayState::PLAYING;
    }
}

void CameraAnimationData::Reset() {
    // 全キーフレームをリセット
    for (auto& keyframe : keyFrames_) {
        keyframe->Reset();
    }

    // 復帰用イージングをリセット
    returnParam_.positionEase.Reset();
    returnParam_.rotationEase.Reset();
    returnParam_.fovEase.Reset();

    // 最初のキーフレームに初期値を設定
    if (!keyFrames_.empty() && activeKeyFrameIndex_ == 0) {
        keyFrames_[0]->SetStartEasing(initialCameraTransform_.position, initialCameraTransform_.rotation, initialCameraTransform_.fov);
    }

    // フラグをリセット
    isAllKeyFramesFinished_           = false;
    returnParam_.isReturningToInitial = false;
    returnParam_.isWaitingForReturn   = false;
    isAllFinished_                    = false;
    lastCompletedKeyFrameIndex_       = -1;
    activeKeyFrameIndex_              = 0;
    resetParam_.currentDelayTimer     = 0.0f;

    playState_ = PlayState::STOPPED;
}

void CameraAnimationData::RegisterParams() {
    // メイン設定
    globalParameter_->Regist(groupName_, "autoReturnToInitial", &returnParam_.autoReturnToInitial);
    globalParameter_->Regist(groupName_, "resetPosEaseType", &resetParam_.posEaseType);
    globalParameter_->Regist(groupName_, "resetRotateEaseType", &resetParam_.rotateEaseType);
    globalParameter_->Regist(groupName_, "resetFovEaseType", &resetParam_.fovEaseType);
    globalParameter_->Regist(groupName_, "resetTimePoint", &resetParam_.timePoint);
    globalParameter_->Regist(groupName_, "returnDelayTime", &resetParam_.delayTime);
    globalParameter_->Regist(groupName_, "timeMode", &timeMode_);
}

void CameraAnimationData::GetParams() {
    returnParam_.autoReturnToInitial = globalParameter_->GetValue<bool>(groupName_, "autoReturnToInitial");
    resetParam_.posEaseType          = globalParameter_->GetValue<int32_t>(groupName_, "resetPosEaseType");
    resetParam_.rotateEaseType       = globalParameter_->GetValue<int32_t>(groupName_, "resetRotateEaseType");
    resetParam_.fovEaseType          = globalParameter_->GetValue<int32_t>(groupName_, "resetFovEaseType");
    resetParam_.timePoint            = globalParameter_->GetValue<float>(groupName_, "resetTimePoint");
    resetParam_.delayTime            = globalParameter_->GetValue<float>(groupName_, "returnDelayTime");
    timeMode_                        = globalParameter_->GetValue<int32_t>(groupName_, "timeMode");
}

void CameraAnimationData::AdjustParam() {
#ifdef _DEBUG

    // アニメーション制御
    if (showAnimationControls_) {
        ImGui::SeparatorText(("Camera Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // 再生制御
        if (ImGui::Button("Play"))
            Play();
        ImGui::SameLine();
        if (ImGui::Button("Pause"))
            Pause();
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
            Reset();

        ImGui::Checkbox("Auto Return to Initial", &returnParam_.autoReturnToInitial);

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
        }

        if (isAllKeyFramesFinished_) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Animation Finished!");
        }

        if (returnParam_.isWaitingForReturn) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Waiting for Return... (%.2f/%.2f)",
                resetParam_.currentDelayTimer, resetParam_.delayTime);
        }
    }

    ImGui::SeparatorText("Reset Param");
    // イージングタイプの設定
    ImGui::DragFloat("Return Delay Time", &resetParam_.delayTime, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Reset Time Point", &resetParam_.timePoint, 0.01f);
    ImGuiEasingTypeSelector("Easing Type Position", resetParam_.posEaseType);
    ImGuiEasingTypeSelector("Easing Type Rotate", resetParam_.rotateEaseType);
    ImGuiEasingTypeSelector("Easing Type Fov", resetParam_.fovEaseType);

    ImGui::SeparatorText("deltaTime");
    TimeModeSelector("Time Mode", timeMode_);

    ImGui::SeparatorText("keyFrameEdit");
    // キーフレームリスト
    if (showKeyFrameList_) {
        ImGui::Text("KeyFrames (%zu):", keyFrames_.size());
        for (int32_t i = 0; i < static_cast<int32_t>(keyFrames_.size()); ++i) {
            ImGui::PushID(i);

            bool isSelected       = (selectedKeyFrameIndex_ == i);
            bool isActive         = (activeKeyFrameIndex_ == i);
            std::string labelText = "KeyFrame " + std::to_string(i) + " (t:" + std::to_string(keyFrames_[i]->GetTimePoint()) + ")";

            // アクティブなキーフレームを強調表示
            if (isActive) {
                labelText += " [ACTIVE]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            }

            if (ImGui::Selectable(labelText.c_str(), isSelected, 0, ImVec2(0, 0))) {
                selectedKeyFrameIndex_ = i;
            }

            if (isActive) {
                ImGui::PopStyleColor();
            }

            ImGui::PopID();
            ImGui::Spacing();
        }

        if (ImGui::Button("Add KeyFrame")) {
            AddKeyFrame();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All KeyFrames")) {
            ClearKeyFrames();
        }
    }
    ImGui::Separator();
    // 選択されたキーフレームの調整
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        keyFrames_[selectedKeyFrameIndex_]->AdjustParam();
    }

    // 現在の値表示
    ImGui::SeparatorText("Current Values");
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", currentCameraTransform_.position.x, currentCameraTransform_.position.y, currentCameraTransform_.position.z);
    ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", currentCameraTransform_.rotation.x, currentCameraTransform_.rotation.y, currentCameraTransform_.rotation.z);
    ImGui::Text("FOV: %.2f", currentCameraTransform_.fov);

    ImGui::PopID();
#endif // _DEBUG
}

void CameraAnimationData::SetInitialValues(const Vector3& position, const Vector3& rotation, const float& fov) {
    initialCameraTransform_.position = position;
    initialCameraTransform_.rotation = rotation;
    initialCameraTransform_.fov      = fov;
}

void CameraAnimationData::StartReturnToInitial() {
    if (returnParam_.isReturningToInitial || returnParam_.isWaitingForReturn) {
        return;
    }

    // 待機時間が設定されている場合は待機状態に
    if (resetParam_.delayTime > 0.0f) {
        returnParam_.isWaitingForReturn = true;
        resetParam_.currentDelayTimer   = 0.0f;
    } else {
        // 待機時間がない場合は即座にイージング開始
        returnParam_.isReturningToInitial = true;
    }

    Vector3 currentPos    = currentCameraTransform_.position;
    Vector3 currentRot    = currentCameraTransform_.rotation;
    float currentFovValue = currentCameraTransform_.fov;

    // 現在の値から初期値へのイージングを設定
    returnParam_.positionEase.SetStartValue(currentPos);
    returnParam_.positionEase.SetEndValue(initialCameraTransform_.position);
    returnParam_.positionEase.SetMaxTime(resetParam_.timePoint);
    returnParam_.positionEase.SetType(static_cast<EasingType>(resetParam_.posEaseType));
    returnParam_.positionEase.Reset();

    returnParam_.rotationEase.SetStartValue(currentRot);
    returnParam_.rotationEase.SetEndValue(initialCameraTransform_.rotation);
    returnParam_.rotationEase.SetMaxTime(resetParam_.timePoint);
    returnParam_.rotationEase.SetType(static_cast<EasingType>(resetParam_.rotateEaseType));
    returnParam_.rotationEase.Reset();

    returnParam_.fovEase.SetStartValue(currentFovValue);
    returnParam_.fovEase.SetEndValue(initialCameraTransform_.fov);
    returnParam_.fovEase.SetMaxTime(resetParam_.timePoint);
    returnParam_.fovEase.SetType(static_cast<EasingType>(resetParam_.fovEaseType));
    returnParam_.fovEase.Reset();
}

void CameraAnimationData::SetSelectedKeyFrameIndex(const int32_t& index) {
    if (index >= -1 && index < static_cast<int32_t>(keyFrames_.size())) {
        selectedKeyFrameIndex_ = index;
    }
}

const CameraKeyFrame* CameraAnimationData::GetSelectedKeyFrame() const {
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        return keyFrames_[selectedKeyFrameIndex_].get();
    }
    return nullptr;
}

bool CameraAnimationData::IsPlaying() const {
    return playState_ == PlayState::PLAYING;
}

void CameraAnimationData::TimeModeSelector(const char* label, int32_t& target) {
    int mode = static_cast<int>(target);
    if (ImGui::Combo(label, &mode, TimeModeLabels.data(), static_cast<int>(TimeModeLabels.size()))) {
        target = mode;
    }
}