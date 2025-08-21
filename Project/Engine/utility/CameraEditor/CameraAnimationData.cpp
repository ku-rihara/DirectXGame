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

    BindParams();

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);
}

void CameraAnimationData::LoadData() {

    // アニメーションデータのロード
    globalParameter_->LoadFile(groupName_, folderName_);
    // キーフレームデータのロード
    LoadAllKeyFrames();
    //値同期
    globalParameter_->SyncParamForGroup(groupName_);
}

void CameraAnimationData::SaveData() {

    // アニメーションデータの保存
    globalParameter_->SaveFile(groupName_, folderName_);
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

                    // このアニメーションのキーフレームファイルかチェック
                    if (fileName.find(keyFramePrefix) == 0) {
                        // インデックス番号を抽出
                        std::string indexStr = fileName.substr(keyFramePrefix.length());
                        try {
                            int32_t index = std::stoi(indexStr);
                            keyFrameFiles.emplace_back(index, fileName);
                        } catch (const std::exception&) {
                            // インデックスの解析に失敗した場合はスキップ
                            continue;
                        }
                    }
                }
            }

            // インデックス順にソート
            std::sort(keyFrameFiles.begin(), keyFrameFiles.end());

            // キーフレームを作成してロード
            for (const auto& [index, fileName] : keyFrameFiles) {
                auto newKeyFrame = std::make_unique<CameraKeyFrame>();
                // 実際のファイルインデックスを使用
                newKeyFrame->Init(groupName_, index);
                newKeyFrame->LoadData(); // キーフレームデータをロード
                keyFrames_.push_back(std::move(newKeyFrame));
            }

            // 最初のキーフレームを選択状態に
            if (!keyFrames_.empty()) {
                selectedKeyFrameIndex_ = 0;
            }
        }
  
}


void CameraAnimationData::Update(float deltaTime) {
    // 再生中の更新
    if (playState_ == PlayState::PLAYING) {
        currentTime_ += deltaTime * playbackSpeed_;
    }

    // キーフレーム間の補間計算
    InterpolateKeyFrames();

    // キーフレームの更新
    for (auto& keyFrame : keyFrames_) {
        keyFrame->Update(deltaTime);
    }
}

void CameraAnimationData::InterpolateKeyFrames() {
    if (keyFrames_.empty()) {
        return;
    }

    // 単一キーフレームの場合
    if (keyFrames_.size() == 1) {
        currentPosition_ = keyFrames_[0]->GetPosition();
        currentRotation_ = keyFrames_[0]->GetRotation();
        currentFov_      = keyFrames_[0]->GetFov();
        return;
    }

    // 現在の時間に基づいて補間するキーフレームを検索
    int32_t fromIndex = -1;
    int32_t toIndex   = -1;

    for (int32_t i = 0; i < static_cast<int32_t>(keyFrames_.size()) - 1; ++i) {
        if (currentTime_ >= keyFrames_[i]->GetTimePoint() && currentTime_ <= keyFrames_[i + 1]->GetTimePoint()) {
            fromIndex = i;
            toIndex   = i + 1;
            break;
        }
    }

    // 範囲外の場合の処理
    if (fromIndex == -1) {
        if (currentTime_ < keyFrames_[0]->GetTimePoint()) {
            // 最初のキーフレーム前
            currentPosition_ = keyFrames_[0]->GetPosition();
            currentRotation_ = keyFrames_[0]->GetRotation();
            currentFov_      = keyFrames_[0]->GetFov();
        } else {
            // 最後のキーフレーム後
            int32_t lastIndex = static_cast<int32_t>(keyFrames_.size()) - 1;
            currentPosition_  = keyFrames_[lastIndex]->GetPosition();
            currentRotation_  = keyFrames_[lastIndex]->GetRotation();
            currentFov_       = keyFrames_[lastIndex]->GetFov();
        }
        return;
    }

    // adapt
}

void CameraAnimationData::ApplyToViewProjection(ViewProjection& viewProjection) {

    // viewProjectionの値適応
    viewProjection.translation_ = currentPosition_;
    viewProjection.rotation_    = currentRotation_;
    viewProjection.fovAngleY_   = currentFov_;

    // 行列更新
    viewProjection.UpdateMatrix();
}

void CameraAnimationData::AddKeyFrame() {
    int32_t newIndex = static_cast<int32_t>(keyFrames_.size());
    auto newKeyFrame = std::make_unique<CameraKeyFrame>();
    newKeyFrame->Init(groupName_, newIndex);

    keyFrames_.push_back(std::move(newKeyFrame));
    selectedKeyFrameIndex_ = newIndex;
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

        // インデックスの再設定
        for (int32_t i = 0; i < static_cast<int32_t>(keyFrames_.size()); ++i) {
            keyFrames_[i]->Init(groupName_, i);
        }
    }
}

void CameraAnimationData::ClearAllKeyFrames() {
    keyFrames_.clear();
    selectedKeyFrameIndex_ = -1;
}

bool CameraAnimationData::IsFinished() const {
    if (keyFrames_.empty()) {
        return false;
    }

    // 最後のキーフレームの時間を取得
    float lastKeyFrameTime = 0.0f;
    for (const auto& keyFrame : keyFrames_) {
        lastKeyFrameTime = std::max(lastKeyFrameTime, keyFrame->GetTimePoint());
    }

    return currentTime_ >= lastKeyFrameTime;
}

void CameraAnimationData::Play() {
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
    currentTime_ = 0.0f;
    playState_   = PlayState::STOPPED;
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

        // 再生制御
        if (ImGui::Button("Play"))
            Play();
        ImGui::SameLine();
        if (ImGui::Button("Pause"))
            Pause();
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
            Reset();

        ImGui::DragFloat("Playback Speed", &playbackSpeed_, 0.1f, 0.1f, 5.0f);
        ImGui::Checkbox("Auto Return to Initial", &autoReturnToInitial_);

        // 時間表示
        ImGui::Text("Current Time: %.2f", currentTime_);

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
        ImGui::Text("State: %s", stateText);
    }

    ImGui::SeparatorText("Reset Param");
    // イージングタイプの設定
    ImGui::DragFloat("Reset Time Point", &resetTimePoint_, 0.01f);
    EasingTypeSelector("Easing Type Position", resetPosEaseType_);
    EasingTypeSelector("Easing Type Rotate", resetRotateEaseType_);
    EasingTypeSelector("Easing Type Fov", resetFovEaseType_);

    // キーフレーム管理
    if (ImGui::CollapsingHeader("KeyFrame Management")) {

        ImGui::Separator();
        // キーフレームリスト
        if (showKeyFrameList_) {
            ImGui::Text("KeyFrames (%zu):", keyFrames_.size());
            for (int32_t i = 0; i < static_cast<int32_t>(keyFrames_.size()); ++i) {
                ImGui::PushID(i);

                bool isSelected       = (selectedKeyFrameIndex_ == i);
                std::string labelText = "KeyFrame " + std::to_string(i) + " (t:" + std::to_string(keyFrames_[i]->GetTimePoint()) + ")";

                if (ImGui::Selectable(labelText.c_str(), isSelected, 0, ImVec2(0, 0))) {
                    selectedKeyFrameIndex_ = i;
                }

               /* ImGui::SameLine();
                if (ImGui::Button("X")) {
                    RemoveKeyFrame(i);
                    ImGui::PopID();
                    break;
                }*/

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
    int type = static_cast<int>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;
    }
}