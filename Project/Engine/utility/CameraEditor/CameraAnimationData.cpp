#include "CameraAnimationData.h"
#undef min
#undef max
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <imgui.h>
#include <iostream>

void CameraAnimationData::Init(const std::string& animationName) {

    globalParameter_ = GlobalParameter::GetInstance();

    // メイングループの設定
    groupName_ = animationName;
    globalParameter_->CreateGroup(groupName_, true);

    // 重複バインドを防ぐ
    globalParameter_->ClearBindingsForGroup(groupName_);
    BindParams();

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);

    activeKeyFrameIndex_ = 0;

    // 新しいフラグの初期化
    isAllKeyFramesFinished_     = false;
    lastCompletedKeyFrameIndex_ = -1;

    returnPositionEase_.SetAdaptValue(&returnPosition_);
    returnRotationEase_.SetAdaptValue(&returnRotation_);
    returnFovEase_.SetAdaptValue(&returnFov_);
}

void CameraAnimationData::LoadData() {

    // アニメーションデータのロード
    globalParameter_->LoadFile(groupName_, folderPath_);
    // キーフレームデータのロード
    LoadAllKeyFrames();
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

void CameraAnimationData::LoadAllKeyFrames() {
    std::string folderPath     = "Resources/GlobalParameter/CameraAnimation/KeyFrames/";
    std::string keyFramePrefix = groupName_;

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // 既存のキーフレームをクリア
        ClearAllKeyFrames();

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

        // キーフレームを作成してロード
        for (const auto& [index, fileName] : keyFrameFiles) {
            auto newKeyFrame = std::make_unique<CameraKeyFrame>();
            newKeyFrame->Init(groupName_, index);
            newKeyFrame->LoadData(); //Load
            keyFrames_.push_back(std::move(newKeyFrame));
        }

        // 最初のキーフレームを選択状態に
        if (!keyFrames_.empty()) {
            selectedKeyFrameIndex_ = 0;

            finalKeyFrameIndex_ = keyFrameFiles.back().first;
        } else {
            finalKeyFrameIndex_ = -1;
        }
    }
}
void CameraAnimationData::Update(float deltaTime) {
    // 再生中の更新
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    // キーフレーム進行管理
    UpdateKeyFrameProgression();

    // アクティブなキーフレームのみ更新
    UpdateActiveKeyFrames(deltaTime * playbackSpeed_);

    // 補間値の更新
    UpdateInterpolatedValues();
}

void CameraAnimationData::UpdateActiveKeyFrames(float deltaTime) {
    if (keyFrames_.empty()) {
        return;
    }

    // 初期値に戻るイージング
    if (isReturningToInitial_) {

        float scaledDeltaTime = deltaTime * playbackSpeed_;
        returnPositionEase_.Update(scaledDeltaTime);
        returnRotationEase_.Update(scaledDeltaTime);
        returnFovEase_.Update(scaledDeltaTime);

        // イージングが完了したかチェック
        if (returnPositionEase_.IsFinished() && returnRotationEase_.IsFinished() && returnFovEase_.IsFinished()) {
            isReturningToInitial_ = false;
            isAllFinished_        = true;
            playState_            = PlayState::STOPPED;
        }
        return;
    }

    // 現在のアクティブキーフレームを更新
    if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        keyFrames_[activeKeyFrameIndex_]->Update();
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

            if (autoReturnToInitial_) {
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
            Vector3 startPos = currentPosition_;
            Vector3 startRot = currentRotation_;
            float startFov   = currentFov_;

            keyFrames_[activeKeyFrameIndex_]->SetStartEasing(startPos, startRot, startFov);
        }
    }
}

void CameraAnimationData::UpdateInterpolatedValues() {
    if (keyFrames_.empty()) {
        return;
    }

    // 初期値復帰中の場合
    if (isReturningToInitial_) {
        currentPosition_ = returnPosition_;
        currentRotation_ = returnRotation_;
        currentFov_      = returnFov_;
    } else if (activeKeyFrameIndex_ >= 0 && activeKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        // アクティブなキーフレームから現在の補間値を取得
        currentPosition_ = keyFrames_[activeKeyFrameIndex_]->GetPosition();
        currentRotation_ = keyFrames_[activeKeyFrameIndex_]->GetRotation();
        currentFov_      = keyFrames_[activeKeyFrameIndex_]->GetFov();
    }
}
void CameraAnimationData::ApplyToViewProjection(ViewProjection& viewProjection) {

    // viewProjectionの値適応
    if (playState_ == PlayState::PLAYING) {
        viewProjection.positionOffset_ = currentPosition_;
        viewProjection.rotationOffset_ = currentRotation_;
        viewProjection.fovAngleY_      = currentFov_;
    } else if (isAllFinished_) {
        viewProjection.positionOffset_ = initialPosition_;
        viewProjection.rotationOffset_ = initialRotation_;
        viewProjection.fovAngleY_      = initialFov_;
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

void CameraAnimationData::RemoveKeyFrame(int32_t index) {
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

void CameraAnimationData::ClearAllKeyFrames() {
    keyFrames_.clear();
    selectedKeyFrameIndex_ = -1;
    activeKeyFrameIndex_   = 0;

    // フラグをリセット
    isAllKeyFramesFinished_ = false;

    lastCompletedKeyFrameIndex_ = -1;
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
    returnPositionEase_.Reset();
    returnRotationEase_.Reset();
    returnFovEase_.Reset();

    // 最初のキーフレームに初期値を設定
    if (!keyFrames_.empty() && activeKeyFrameIndex_ == 0) {
        keyFrames_[0]->SetStartEasing(initialPosition_, initialRotation_, initialFov_);
    }

    // フラグをリセット
    isAllKeyFramesFinished_     = false;
    isReturningToInitial_       = false;
    isAllFinished_              = false;
    lastCompletedKeyFrameIndex_ = -1;
    activeKeyFrameIndex_        = 0;

    playState_ = PlayState::STOPPED;
}

void CameraAnimationData::BindParams() {
    // メイン設定
    globalParameter_->Bind(groupName_, "playbackSpeed", &playbackSpeed_);
    globalParameter_->Bind(groupName_, "autoReturnToInitial", &autoReturnToInitial_);
    globalParameter_->Bind(groupName_, "resetPosEaseType", &resetPosEaseType_);
    globalParameter_->Bind(groupName_, "resetRotateEaseType", &resetRotateEaseType_);
    globalParameter_->Bind(groupName_, "resetFovEaseType", &resetFovEaseType_);
    globalParameter_->Bind(groupName_, "resetTimePoint", &resetTimePoint_);
}

void CameraAnimationData::AdjustParam() {
#ifdef _DEBUG

    // アニメーション制御
    if (showAnimationControls_) {
        ImGui::SeparatorText(("Camera Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

       

        ImGui::DragFloat("Playback Speed", &playbackSpeed_, 0.1f, 0.1f, 5.0f);
        ImGui::Checkbox("Auto Return to Initial", &autoReturnToInitial_);

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
    }

    ImGui::SeparatorText("Reset Param");
    // イージングタイプの設定
    ImGui::DragFloat("Reset Time Point", &resetTimePoint_, 0.01f);
    EasingTypeSelector("Easing Type Position", resetPosEaseType_);
    EasingTypeSelector("Easing Type Rotate", resetRotateEaseType_);
    EasingTypeSelector("Easing Type Fov", resetFovEaseType_);

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
            ClearAllKeyFrames();
        }
    }
    ImGui::Separator();
    // 選択されたキーフレームの調整
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        keyFrames_[selectedKeyFrameIndex_]->AdjustParam();
    }

    // セーブ、ロード
    if (ImGui::Button("Load Data")) {
        LoadData();
        MessageBoxA(nullptr, "Animation data loaded successfully.", "Camera Animation", 0);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Data")) {
        SaveData();
        MessageBoxA(nullptr, "Animation data saved successfully.", "Camera Animation", 0);
    }

    // 現在の値表示
    ImGui::SeparatorText("Current Values");
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", currentPosition_.x, currentPosition_.y, currentPosition_.z);
    ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", currentRotation_.x, currentRotation_.y, currentRotation_.z);
    ImGui::Text("FOV: %.2f", currentFov_);

    ImGui::PopID();
#endif // _DEBUG
}

void CameraAnimationData::EasingTypeSelector(const char* label, int32_t& target) {
    int type = static_cast<int32_t>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;
    }
}

void CameraAnimationData::SetInitialValues(const Vector3& position, const Vector3& rotation, float fov) {
    initialPosition_ = position;
    initialRotation_ = rotation;
    initialFov_      = fov;
}

void CameraAnimationData::StartReturnToInitial() {
    if (isReturningToInitial_) {
        return;
    }
    isReturningToInitial_ = true;

    Vector3 currentPos    = currentPosition_;
    Vector3 currentRot    = currentRotation_;
    float currentFovValue = currentFov_;

    // 現在の値から初期値へのイージングを設定
    returnPositionEase_.SetStartValue(currentPos);
    returnPositionEase_.SetEndValue(initialPosition_);
    returnPositionEase_.SetMaxTime(resetTimePoint_);
    returnPositionEase_.SetType(static_cast<EasingType>(resetPosEaseType_));
    returnPositionEase_.Reset();

    returnRotationEase_.SetStartValue(currentRot);
    returnRotationEase_.SetEndValue(initialRotation_);
    returnRotationEase_.SetMaxTime(resetTimePoint_);
    returnRotationEase_.SetType(static_cast<EasingType>(resetRotateEaseType_));
    returnRotationEase_.Reset();

    returnFovEase_.SetStartValue(currentFovValue);
    returnFovEase_.SetEndValue(initialFov_);
    returnFovEase_.SetMaxTime(resetTimePoint_);
    returnFovEase_.SetType(static_cast<EasingType>(resetFovEaseType_));
    returnFovEase_.Reset();
}

void CameraAnimationData::SetSelectedKeyFrameIndex(int32_t index) {
    if (index >= -1 && index < static_cast<int32_t>(keyFrames_.size())) {
        selectedKeyFrameIndex_ = index;
    }
}
CameraKeyFrame* CameraAnimationData::GetSelectedKeyFrame() {
    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
        return keyFrames_[selectedKeyFrameIndex_].get();
    }
    return nullptr;
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