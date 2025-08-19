#include "CameraAnimationData.h"
#undef min
#undef max
#include <algorithm>
#include <cmath>
#include <imgui.h>

void CameraAnimationData::Init(const std::string& animationName) {

    globalParameter_ = GlobalParameter::GetInstance();

    // メイングループの設定
    groupName_ = animationName;
    globalParameter_->CreateGroup(groupName_, true);

    BindParams();

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);
}

void CameraAnimationData::Update(float deltaTime) {
    // 再生中の更新
    if (playState_ == PlayState::PLAYING) {
        currentTime_ += deltaTime * playbackSpeed_;
    }

    // キーフレームの更新
    for (auto& keyFrame : keyFrames_) {
        keyFrame->Update(deltaTime);
    }
}

void CameraAnimationData::ApplyToViewProjection(ViewProjection& viewProjection) {
   
    // viewProjectionの値適応
    viewProjection.translation_ = currentPosition_; 
    viewProjection.rotation_ = currentRotation_;
    viewProjection.fovAngleY_ = currentFov_;

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
    currentTime_          = 0.0f;
    playState_            = PlayState::STOPPED;
}

void CameraAnimationData::BindParams() {
    // メイン設定
    globalParameter_->Bind(groupName_, "playbackSpeed", &playbackSpeed_);
    globalParameter_->Bind(groupName_, "autoReturnToInitial", &autoReturnToInitial_);
    globalParameter_->Bind(groupName_, "newKeyFrameTime", &newKeyFrameTime_);
    globalParameter_->Bind(groupName_, "resetPosEaseType", &resetPosEaseType_);
    globalParameter_->Bind(groupName_, "resetRotateEaseType", &resetRotateEaseType_);
    globalParameter_->Bind(groupName_, "resetFovEaseType", &resetFovEaseType_);
   
}

void CameraAnimationData::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(("Camera Editor: " + groupName_).c_str())) {
        ImGui::PushID(groupName_.c_str());

        globalParameter_->SaveFile(groupName_, folderName_);
        globalParameter_->LoadFile(groupName_, folderName_);

        ImGui::Separator();

        // アニメーション制御
        if (showAnimationControls_) {
            if (ImGui::CollapsingHeader("Animation Controls")) {
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

                // リセットに関するパラメータ

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
        }

        // キーフレーム管理
        if (ImGui::CollapsingHeader("KeyFrame Management")) {
            ImGui::DragFloat("New KeyFrame Time", &newKeyFrameTime_, 0.1f, 0.0f, 1000.0f);
            if (ImGui::Button("Add KeyFrame")) {
                AddKeyFrame();
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear All KeyFrames")) {
                ClearAllKeyFrames();
            }

            // キーフレームリスト
            if (showKeyFrameList_) {
                ImGui::Text("KeyFrames (%zu):", keyFrames_.size());
                for (int32_t i = 0; i < static_cast<int32_t>(keyFrames_.size()); ++i) {
                    ImGui::PushID(i);

                    bool isSelected = (selectedKeyFrameIndex_ == i);
                    if (ImGui::Selectable(("KeyFrame " + std::to_string(i)).c_str(), isSelected)) {
                        selectedKeyFrameIndex_ = i;
                    }

                    ImGui::SameLine();
                    if (ImGui::SmallButton("X")) {
                        RemoveKeyFrame(i);
                        ImGui::PopID();
                        break;
                    }

                    ImGui::PopID();
                }
            }
        }

        // 選択されたキーフレームの調整
        if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < static_cast<int32_t>(keyFrames_.size())) {
            keyFrames_[selectedKeyFrameIndex_]->AdjustParam();
        }

        // 現在の値表示
        if (ImGui::CollapsingHeader("Current Values")) {
            ImGui::Text("Position: (%.2f, %.2f, %.2f)", currentPosition_.x, currentPosition_.y, currentPosition_.z);
            ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", currentRotation_.x, currentRotation_.y, currentRotation_.z);
            ImGui::Text("FOV: %.2f", currentFov_);
        }

        ImGui::PopID();
    }
#endif // _DEBUG
}

