#include "CameraEditor.h"
#include <algorithm>
#include <cmath>
#include <imgui.h>

void CameraEditor::Init(const std::string& animationName) {
    animationName_   = animationName;
    globalParameter_ = GlobalParameter::GetInstance();

    // メイングループの設定
    groupName_ = "CameraEditor/" + animationName_;
    globalParameter_->CreateGroup(groupName_, true);

    BindParams();

    // 初期設定とメイン設定を読み込み
    LoadInitialSettings();
    LoadAnimation();

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);

    // 初期値設定
    currentPosition_ = initialSettings_.position;
    currentRotation_ = initialSettings_.rotation;
    currentFov_      = initialSettings_.fov;

    // 復帰用イージング設定
    returnPositionEase_.SetType(static_cast<EasingType>(initialSettings_.returnEaseType));
    returnRotationEase_.SetType(static_cast<EasingType>(initialSettings_.returnEaseType));
    returnFovEase_.SetType(static_cast<EasingType>(initialSettings_.returnEaseType));

    returnPositionEase_.SetAdaptValue(&currentPosition_);
    returnRotationEase_.SetAdaptValue(&currentRotation_);
    returnFovEase_.SetAdaptValue(&currentFov_);
}

void CameraEditor::Update(float deltaTime) {
    // 再生中の更新
    if (playState_ == PlayState::PLAYING) {
        currentTime_ += deltaTime * playbackSpeed_;

        // ループ処理
        if (IsFinished()) {
            if (loopAnimation_) {
                currentTime_ = 0.0f;
            } else {
                Stop();
                if (autoReturnToInitial_) {
                    // 初期値復帰開始
                    isReturningToInitial_ = true;
                    returnTimer_          = 0.0f;
                    returnStartPosition_  = currentPosition_;
                    returnStartRotation_  = currentRotation_;
                    returnStartFov_       = currentFov_;

                    returnPositionEase_.SetMaxTime(initialSettings_.returnDuration);
                    returnRotationEase_.SetMaxTime(initialSettings_.returnDuration);
                    returnFovEase_.SetMaxTime(initialSettings_.returnDuration);

                    returnPositionEase_.Start(returnStartPosition_, initialSettings_.position);
                    returnRotationEase_.Start(returnStartRotation_, initialSettings_.rotation);
                    returnFovEase_.Start(returnStartFov_, initialSettings_.fov);
                }
                return;
            }
        }

        UpdateInterpolation();
    }

    // 初期値復帰処理
    if (isReturningToInitial_) {
        UpdateReturnToInitial(deltaTime);
    }

    // キーフレームの更新
    for (auto& keyFrame : keyFrames_) {
        keyFrame->Update(deltaTime);
    }
}

void CameraEditor::UpdateInterpolation() {
    if (keyFrames_.empty()) {
        return;
    }

    // キーフレームを時間順にソート
    SortKeyFramesByTime();

    // 現在時刻に対応するキーフレームを取得
    auto [prevIndex, nextIndex] = GetSurroundingKeyFrames();

    if (prevIndex == -1 && nextIndex == -1) {
        // キーフレームなし
        return;
    } else if (prevIndex == -1) {
        // 最初のキーフレーム前
        currentPosition_ = initialSettings_.position;
        currentRotation_ = initialSettings_.rotation;
        currentFov_      = initialSettings_.fov;
    } else if (nextIndex == -1) {
        // 最後のキーフレーム後
        // 最後のキーフレームの値を維持
        // キーフレームから値を取得する必要があるが、CameraKeyFrameに getter がないため
        // ここでは最後のキーフレームの値で固定
    } else if (prevIndex == nextIndex) {
        // キーフレーム上
        // 同様にキーフレームの値を取得
    } else {
        // 2つのキーフレーム間で補間
        CalculateInterpolatedValues();
    }
}

void CameraEditor::UpdateReturnToInitial(float deltaTime) {
    returnTimer_ += deltaTime;

    returnPositionEase_.Update(deltaTime);
    returnRotationEase_.Update(deltaTime);
    returnFovEase_.Update(deltaTime);

    if (returnTimer_ >= initialSettings_.returnDuration) {
        isReturningToInitial_ = false;
        currentPosition_      = initialSettings_.position;
        currentRotation_      = initialSettings_.rotation;
        currentFov_           = initialSettings_.fov;
    }
}

void CameraEditor::ApplyToViewProjection(ViewProjection& viewProjection) {
    // 位置設定
    viewProjection.translation_ = currentPosition_;

    // 回転をQuaternionに変換してから適用
    // ただし、ViewProjectionはEuler角を使用しているため、直接設定
    viewProjection.rotation_ = currentRotation_;

    // FOV設定
    viewProjection.fovAngleY_ = currentFov_;

    // 行列更新
    viewProjection.UpdateMatrix();
}

void CameraEditor::AddKeyFrame() {
    int32_t newIndex = static_cast<int32_t>(keyFrames_.size());
    auto newKeyFrame = std::make_unique<CameraKeyFrame>();
    newKeyFrame->Init(animationName_, newIndex);

    keyFrames_.push_back(std::move(newKeyFrame));
    selectedKeyFrameIndex_ = newIndex;
}

void CameraEditor::RemoveKeyFrame(int32_t index) {
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
            keyFrames_[i]->Init(animationName_, i);
        }
    }
}

void CameraEditor::ClearAllKeyFrames() {
    keyFrames_.clear();
    selectedKeyFrameIndex_ = -1;
}

void CameraEditor::Play() {
    playState_            = PlayState::PLAYING;
    isReturningToInitial_ = false;
}

void CameraEditor::Stop() {
    playState_            = PlayState::STOPPED;
    isReturningToInitial_ = false;
}

void CameraEditor::Pause() {
    if (playState_ == PlayState::PLAYING) {
        playState_ = PlayState::PAUSED;
    } else if (playState_ == PlayState::PAUSED) {
        playState_ = PlayState::PLAYING;
    }
}

void CameraEditor::Reset() {
    currentTime_          = 0.0f;
    playState_            = PlayState::STOPPED;
    isReturningToInitial_ = false;
    currentPosition_      = initialSettings_.position;
    currentRotation_      = initialSettings_.rotation;
    currentFov_           = initialSettings_.fov;
}

void CameraEditor::SetCurrentTime(float time) {
    currentTime_ = max(0.0f, std::min(time, GetTotalDuration()));
    if (playState_ == PlayState::PLAYING || playState_ == PlayState::PAUSED) {
        UpdateInterpolation();
    }
}

float CameraEditor::GetTotalDuration() const {
    float maxTime = 0.0f;
    // 全キーフレームから最大時間を取得
    // CameraKeyFrameに時間取得のgetterが必要
    return maxTime;
}

bool CameraEditor::IsFinished() const {
    return currentTime_ >= GetTotalDuration();
}

void CameraEditor::BindParams() {
    // メイン設定
    globalParameter_->Bind(groupName_, "playbackSpeed", &playbackSpeed_);
    globalParameter_->Bind(groupName_, "loopAnimation", &loopAnimation_);
    globalParameter_->Bind(groupName_, "autoReturnToInitial", &autoReturnToInitial_);
    globalParameter_->Bind(groupName_, "newKeyFrameTime", &newKeyFrameTime_);
    globalParameter_->Bind(groupName_, "returnDuration", &initialSettings_.returnDuration);
    globalParameter_->Bind(groupName_, "returnEaseType", &initialSettings_.returnEaseType);

    //// 初期設定
    // std::string initialGroupName = groupName_ + "_Initial";
    // globalParameter_->Bind(initialGroupName, "position", &initialSettings_.position);
    // globalParameter_->Bind(initialGroupName, "rotation", &initialSettings_.rotation);
    // globalParameter_->Bind(initialGroupName, "fov", &initialSettings_.fov);
    // globalParameter_->Bind(initialGroupName, "returnDuration", &initialSettings_.returnDuration);
    // globalParameter_->Bind(initialGroupName, "returnEaseType", &initialSettings_.returnEaseType);
}

void CameraEditor::SaveAnimation() {
    globalParameter_->SaveFile(groupName_, "CameraAnimation");
}

void CameraEditor::LoadAnimation() {
    globalParameter_->LoadFile(groupName_, "CameraAnimation");
}

void CameraEditor::SaveInitialSettings() {
    std::string initialGroupName = groupName_ + "_Initial";
    globalParameter_->SaveFile(initialGroupName, "CameraAnimation");
}

void CameraEditor::LoadInitialSettings() {
    std::string initialGroupName = groupName_ + "_Initial";
    globalParameter_->LoadFile(initialGroupName, "CameraAnimation");
}

void CameraEditor::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(("Camera Editor: " + animationName_).c_str())) {
        ImGui::PushID(animationName_.c_str());

        // セーブ・ロード
        if (ImGui::Button("Save Animation")) {
            SaveAnimation();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load Animation")) {
            LoadAnimation();
        }

        ImGui::Separator();

        // アニメーション制御
        if (showAnimationControls_) {
            if (ImGui::CollapsingHeader("Animation Controls")) {
                // 再生制御
                if (ImGui::Button("Play"))
                    Play();
                ImGui::SameLine();
                if (ImGui::Button("Stop"))
                    Stop();
                ImGui::SameLine();
                if (ImGui::Button("Pause"))
                    Pause();
                ImGui::SameLine();
                if (ImGui::Button("Reset"))
                    Reset();

                // 時間制御
                float totalDuration = GetTotalDuration();
                if (ImGui::SliderFloat("Current Time", &currentTime_, 0.0f, totalDuration)) {
                    SetCurrentTime(currentTime_);
                }

                ImGui::DragFloat("Playback Speed", &playbackSpeed_, 0.1f, 0.1f, 5.0f);
                ImGui::Checkbox("Loop Animation", &loopAnimation_);
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
                ImGui::Text("State: %s", stateText);

                if (isReturningToInitial_) {
                    ImGui::Text("Returning to Initial (%.2f/%.2f)", returnTimer_, initialSettings_.returnDuration);
                }
            }
        }

        // 初期設定
        if (ImGui::CollapsingHeader("Initial Settings")) {
            if (ImGui::Button("Save Initial Settings")) {
                SaveInitialSettings();
            }
            ImGui::SameLine();
            if (ImGui::Button("Load Initial Settings")) {
                LoadInitialSettings();
            }

            ImGui::DragFloat("Return Duration", &initialSettings_.returnDuration, 0.1f, 0.1f, 10.0f);

            // イージングタイプ選択
            int easeType = initialSettings_.returnEaseType;
            if (ImGui::Combo("Return Ease Type", &easeType, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
                initialSettings_.returnEaseType = easeType;
                returnPositionEase_.SetType(static_cast<EasingType>(easeType));
                returnRotationEase_.SetType(static_cast<EasingType>(easeType));
                returnFovEase_.SetType(static_cast<EasingType>(easeType));
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

void CameraEditor::SortKeyFramesByTime() {
    // CameraKeyFrameに時間取得のgetterが必要
    // 今は仮実装
}

std::pair<int32_t, int32_t> CameraEditor::GetSurroundingKeyFrames() const {
    // CameraKeyFrameの時間を取得して、currentTime_を囲むキーフレームを探す
    // 今は仮実装
    return {-1, -1};
}

void CameraEditor::CalculateInterpolatedValues() {
    // 2つのキーフレーム間での補間計算
    // CameraKeyFrameから値を取得して補間する必要がある
}

Quaternion CameraEditor::EulerToQuaternion(const Vector3& euler) {
    return Quaternion::MakeRotateAxisAngle({0, 1, 0}, euler.y) * Quaternion::MakeRotateAxisAngle({1, 0, 0}, euler.x) * Quaternion::MakeRotateAxisAngle({0, 0, 1}, euler.z);
}

Vector3 CameraEditor::QuaternionToEuler(const Quaternion& quaternion) {
    // Quaternionからオイラー角への変換
    float sinr_cosp = 2 * (quaternion.w * quaternion.x + quaternion.y * quaternion.z);
    float cosr_cosp = 1 - 2 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
    float x         = std::atan2(sinr_cosp, cosr_cosp);

    float sinp = 2 * (quaternion.w * quaternion.y - quaternion.z * quaternion.x);
    float y    = std::abs(sinp) >= 1 ? std::copysign(3.141592654f / 2, sinp) : std::asin(sinp);

    float siny_cosp = 2 * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
    float cosy_cosp = 1 - 2 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
    float z         = std::atan2(siny_cosp, cosy_cosp);

    return Vector3(x, y, z);
}