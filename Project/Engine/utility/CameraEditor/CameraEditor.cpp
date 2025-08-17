#include "CameraEditor.h"
#include "CameraAnimationSerializer.h"
#include <algorithm>
#include <imgui.h>
#include <iostream>

CameraEditor::CameraEditor() {
}

void CameraEditor::Init(ViewProjection* viewProjection) {
    viewProjection_ = viewProjection;

    // イージング生成
    positionEasing_ = std::make_unique<Easing<Vector3>>();
    rotationEasing_ = std::make_unique<Easing<Vector3>>();
    fovEasing_      = std::make_unique<Easing<float>>();
    serializer_     = std::make_unique<CameraAnimationSerializer>();

    // イージング初期化
    positionEasing_->Init("CameraPosition");
    rotationEasing_->Init("CameraRotation");
    fovEasing_->Init("CameraFOV");

    // デフォルトアニメーション作成
    CreateNewAnimation("DefaultAnimation");

    // 現在のカメラパラメータを保存
    SaveOriginalCameraParams();
}

void CameraEditor::Update(float deltaTime) {
    if (isPlaying_) {
        UpdateAnimation(deltaTime * playbackSpeed_);
    }
}

void CameraEditor::UpdateAnimation(float deltaTime) {
    if (currentAnimationIndex_ < 0 || currentAnimationIndex_ >= animations_.size()) {
        return;
    }

    auto& currentAnimation = animations_[currentAnimationIndex_];

    if (currentAnimation.keyFrames.empty()) {
        return;
    }

    currentTime_ += deltaTime;

    // アニメーション終了チェック
    if (currentTime_ >= currentAnimation.totalDuration) {

        currentTime_ = currentAnimation.totalDuration;
        isPlaying_   = false;
        if (onAnimationFinishCallback_) {
            onAnimationFinishCallback_();
        }
        return;
    }

    // カメラ補間
    InterpolateCamera(currentTime_);
}

void CameraEditor::InterpolateCamera(float time) {
    if (currentAnimationIndex_ < 0 || currentAnimationIndex_ >= animations_.size()) {
        return;
    }

    auto& animation = animations_[currentAnimationIndex_];

    if (animation.keyFrames.size() <= 1) {
        if (!animation.keyFrames.empty()) {
            auto& keyFrame                = animation.keyFrames[0];
            viewProjection_->translation_ = keyFrame.position;
            viewProjection_->rotation_    = keyFrame.rotation;
            viewProjection_->fovAngleY_   = keyFrame.fov;
        }
        return;
    }

    // 現在時間に対応するキーフレームを見つける
    int32_t fromIndex = -1;
    int32_t toIndex   = -1;

    for (int32_t i = 0; i < animation.keyFrames.size() - 1; ++i) {
        if (time >= animation.keyFrames[i].timePoint && time <= animation.keyFrames[i + 1].timePoint) {
            fromIndex = i;
            toIndex   = i + 1;
            break;
        }
    }

    // 見つからない場合は最後のキーフレームを使用
    if (fromIndex == -1) {
        if (time <= animation.keyFrames[0].timePoint) {
            fromIndex = toIndex = 0;
        } else {
            fromIndex = toIndex = static_cast<int32_t>(animation.keyFrames.size() - 1);
        }
    }

    // 新しいセグメントに入った場合、イージングを設定
    if (currentFromKeyFrame_ != fromIndex || currentToKeyFrame_ != toIndex) {
        SetupEasingForSegment(fromIndex, toIndex);
        currentFromKeyFrame_ = fromIndex;
        currentToKeyFrame_   = toIndex;
    }

    if (fromIndex == toIndex) {
        // 単一キーフレーム
        auto& keyFrame                = animation.keyFrames[fromIndex];
        viewProjection_->translation_ = keyFrame.position;
        viewProjection_->rotation_    = keyFrame.rotation;
        viewProjection_->fovAngleY_   = keyFrame.fov;
    } else {
        // 補間
        auto& fromFrame = animation.keyFrames[fromIndex];
        auto& toFrame   = animation.keyFrames[toIndex];

        float segmentTime     = time - fromFrame.timePoint;
        float segmentDuration = toFrame.timePoint - fromFrame.timePoint;

        if (segmentDuration > 0.0f) {
            float t = segmentTime / segmentDuration;

            // イージングを使用して補間
            positionEasing_->SetCurrentValue(Vector3(
                fromFrame.position.x + (toFrame.position.x - fromFrame.position.x) * t,
                fromFrame.position.y + (toFrame.position.y - fromFrame.position.y) * t,
                fromFrame.position.z + (toFrame.position.z - fromFrame.position.z) * t));

            rotationEasing_->SetCurrentValue(Vector3(
                fromFrame.rotation.x + (toFrame.rotation.x - fromFrame.rotation.x) * t,
                fromFrame.rotation.y + (toFrame.rotation.y - fromFrame.rotation.y) * t,
                fromFrame.rotation.z + (toFrame.rotation.z - fromFrame.rotation.z) * t));

            fovEasing_->SetCurrentValue(fromFrame.fov + (toFrame.fov - fromFrame.fov) * t);

            viewProjection_->translation_ = positionEasing_->GetValue();
            viewProjection_->rotation_    = rotationEasing_->GetValue();
            viewProjection_->fovAngleY_   = fovEasing_->GetValue();
        }
    }
}

void CameraEditor::SetupEasingForSegment(int32_t fromKeyFrameIndex, int32_t toKeyFrameIndex) {
    if (currentAnimationIndex_ < 0 || fromKeyFrameIndex < 0 || toKeyFrameIndex < 0) {
        return;
    }

    auto& animation = animations_[currentAnimationIndex_];
    if (fromKeyFrameIndex >= animation.keyFrames.size() || toKeyFrameIndex >= animation.keyFrames.size()) {
        return;
    }

    auto& fromFrame = animation.keyFrames[fromKeyFrameIndex];
    auto& toFrame   = animation.keyFrames[toKeyFrameIndex];

    float duration = toFrame.timePoint - fromFrame.timePoint;

    // Position Easing
    EasingParameter<Vector3> posParam;
    posParam.type       = fromFrame.positionEasingType;
    posParam.startValue = fromFrame.position;
    posParam.endValue   = toFrame.position;
    posParam.maxTime    = duration;
    posParam.finishType = EasingFinishValueType::End;

    positionEasing_->SettingValue(posParam);
    positionEasing_->SetAdaptValue(&viewProjection_->translation_);
    positionEasing_->Reset();

    // Rotation Easing
    EasingParameter<Vector3> rotParam;
    rotParam.type       = fromFrame.rotationEasingType;
    rotParam.startValue = fromFrame.rotation;
    rotParam.endValue   = toFrame.rotation;
    rotParam.maxTime    = duration;
    rotParam.finishType = EasingFinishValueType::End;

    rotationEasing_->SettingValue(rotParam);
    rotationEasing_->SetAdaptValue(&viewProjection_->rotation_);
    rotationEasing_->Reset();

    // FOV Easing
    EasingParameter<float> fovParam;
    fovParam.type       = fromFrame.fovEasingType;
    fovParam.startValue = fromFrame.fov;
    fovParam.endValue   = toFrame.fov;
    fovParam.maxTime    = duration;
    fovParam.finishType = EasingFinishValueType::End;

    fovEasing_->SettingValue(fovParam);
    fovEasing_->SetAdaptValue(&viewProjection_->fovAngleY_);
    fovEasing_->Reset();
}

void CameraEditor::DrawImGui() {
#ifdef _DEBUG
    if (ImGui::Begin("Camera Editor")) {
        DrawAnimationControls();
        ImGui::Separator();
        DrawKeyFrameList();
        ImGui::Separator();
        DrawKeyFrameEditor();
        ImGui::Separator();
        DrawFileOperations();
        ImGui::Separator();
        DrawCameraPreview();
    }
    ImGui::End();
#endif
}

void CameraEditor::DrawAnimationControls() {
    // アニメーション選択
    if (ImGui::BeginCombo("Animation", currentAnimationName_.c_str())) {
        for (int32_t i = 0; i < animations_.size(); ++i) {
            bool isSelected = (currentAnimationName_ == animations_[i].name);
            if (ImGui::Selectable(animations_[i].name.c_str(), isSelected)) {
                currentAnimationName_  = animations_[i].name;
                currentAnimationIndex_ = i;
                StopAnimation();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // 再生コントロール
    ImGui::SameLine();
    if (ImGui::Button(isPlaying_ ? "Stop" : "Play")) {
        if (isPlaying_) {
            StopAnimation();
        } else {
            PlayAnimation(currentAnimationName_);
        }
    }

    // タイムライン
    if (currentAnimationIndex_ >= 0) {
        auto& animation = animations_[currentAnimationIndex_];
        if (ImGui::SliderFloat("Time", &currentTime_, 0.0f, animation.totalDuration)) {
            SetAnimationTime(currentTime_);
        }

        ImGui::SliderFloat("Speed", &playbackSpeed_, 0.1f, 3.0f);
    }

    // 新規アニメーション作成
    ImGui::InputText("New Animation Name", newAnimationNameBuffer_, sizeof(newAnimationNameBuffer_));
    ImGui::SameLine();
    if (ImGui::Button("Create")) {
        CreateNewAnimation(newAnimationNameBuffer_);
        strcpy_s(newAnimationNameBuffer_, "NewAnimation");
    }
}

void CameraEditor::DrawKeyFrameList() {
    if (currentAnimationIndex_ < 0)
        return;

    auto& animation = animations_[currentAnimationIndex_];

    ImGui::Text("KeyFrames:");

    if (ImGui::Button("Add KeyFrame")) {
        AddKeyFrame(currentTime_);
    }

    ImGui::SameLine();
    if (ImGui::Button("Update from Current Camera") && selectedKeyFrameIndex_ >= 0) {
        UpdateKeyFrameFromCurrentCamera(selectedKeyFrameIndex_);
    }

    // キーフレームリスト
    if (ImGui::BeginListBox("##keyframes")) {
        for (int32_t i = 0; i < animation.keyFrames.size(); ++i) {
            auto& keyFrame  = animation.keyFrames[i];
            bool isSelected = (selectedKeyFrameIndex_ == i);

            char label[256];
            sprintf_s(label, "KeyFrame %d: %.2fs", i, keyFrame.timePoint);

            if (ImGui::Selectable(label, isSelected)) {
                selectedKeyFrameIndex_ = i;
            }

            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    if (selectedKeyFrameIndex_ >= 0 && selectedKeyFrameIndex_ < animation.keyFrames.size()) {
        if (ImGui::Button("Delete Selected")) {
            DeleteKeyFrame(selectedKeyFrameIndex_);
            selectedKeyFrameIndex_ = -1;
        }

        ImGui::SameLine();
        if (ImGui::Button("Apply to Camera")) {
            ApplyKeyFrameToCamera(selectedKeyFrameIndex_);
        }
    }
}

void CameraEditor::DrawKeyFrameEditor() {
    if (currentAnimationIndex_ < 0 || selectedKeyFrameIndex_ < 0)
        return;

    auto& animation = animations_[currentAnimationIndex_];
    if (selectedKeyFrameIndex_ >= animation.keyFrames.size())
        return;

    auto& keyFrame = animation.keyFrames[selectedKeyFrameIndex_];

    ImGui::Text("KeyFrame Editor:");

    bool changed = false;
    changed |= ImGui::DragFloat("Time Point", &keyFrame.timePoint, 0.1f, 0.0f, 999.0f);
    changed |= ImGui::DragFloat3("Position", &keyFrame.position.x, 0.1f);
    changed |= ImGui::DragFloat3("Rotation", &keyFrame.rotation.x, 0.01f);
    changed |= ImGui::SliderFloat("FOV", &keyFrame.fov, 0.1f, 3.14f);

    // イージングタイプ選択
    int32_t posEasingIndex = static_cast<int32_t>(keyFrame.positionEasingType);
    int32_t rotEasingIndex = static_cast<int32_t>(keyFrame.rotationEasingType);
    int32_t fovEasingIndex = static_cast<int32_t>(keyFrame.fovEasingType);

    if (ImGui::Combo("Position Easing", &posEasingIndex, EasingTypeLabels.data(), static_cast<int32_t>(EasingType::COUNT))) {
        keyFrame.positionEasingType = static_cast<EasingType>(posEasingIndex);
        changed                     = true;
    }

    if (ImGui::Combo("Rotation Easing", &rotEasingIndex, EasingTypeLabels.data(), static_cast<int32_t>(EasingType::COUNT))) {
        keyFrame.rotationEasingType = static_cast<EasingType>(rotEasingIndex);
        changed                     = true;
    }

    if (ImGui::Combo("FOV Easing", &fovEasingIndex, EasingTypeLabels.data(), static_cast<int32_t>(EasingType::COUNT))) {
        keyFrame.fovEasingType = static_cast<EasingType>(fovEasingIndex);
        changed                = true;
    }

    if (changed) {
        SortKeyFramesByTime();
        RecalculateTotalDuration();
    }
}

void CameraEditor::DrawFileOperations() {
    ImGui::Text("File Operations:");

    ImGui::InputText("File Path", filePathBuffer_, sizeof(filePathBuffer_));

    if (ImGui::Button("Save All Animations")) {
        SaveAnimationsToJson(filePathBuffer_);
    }

    ImGui::SameLine();
    if (ImGui::Button("Load Animations")) {
        LoadAnimationsFromJson(filePathBuffer_);
    }

    if (ImGui::Button("Export Current Animation")) {
        std::string path = std::string(filePathBuffer_) + currentAnimationName_ + ".json";
        ExportCurrentAnimationToJson(path);
    }

    ImGui::SameLine();
    if (ImGui::Button("Import Animation")) {
        ImportAnimationFromJson(filePathBuffer_);
    }
}

void CameraEditor::DrawCameraPreview() {
    if (ImGui::Button("Save Original Camera")) {
        SaveOriginalCameraParams();
    }

    ImGui::SameLine();
    if (ImGui::Button("Restore Original Camera")) {
        RestoreOriginalCameraParams();
    }

    // 現在のカメラパラメータ表示
    ImGui::Text("Current Camera:");
    ImGui::Text("Position: %.2f, %.2f, %.2f",
        viewProjection_->translation_.x,
        viewProjection_->translation_.y,
        viewProjection_->translation_.z);
    ImGui::Text("Rotation: %.3f, %.3f, %.3f",
        viewProjection_->rotation_.x,
        viewProjection_->rotation_.y,
        viewProjection_->rotation_.z);
    ImGui::Text("FOV: %.3f", viewProjection_->fovAngleY_);
}

void CameraEditor::PlayAnimation(const std::string& animationName) {
    auto it = std::find_if(animations_.begin(), animations_.end(),
        [&animationName](const CameraAnimation& anim) {
            return anim.name == animationName;
        });

    if (it != animations_.end()) {
        currentAnimationName_  = animationName;
        currentAnimationIndex_ = static_cast<int32_t>(std::distance(animations_.begin(), it));
        currentTime_           = 0.0f;
        isPlaying_             = true;
        currentFromKeyFrame_   = -1;
        currentToKeyFrame_     = -1;
    }
}

void CameraEditor::StopAnimation() {
    isPlaying_           = false;
    currentTime_         = 0.0f;
    currentFromKeyFrame_ = -1;
    currentToKeyFrame_   = -1;
}

void CameraEditor::SetAnimationTime(float time) {
    currentTime_ = time;
    if (!isPlaying_ && currentAnimationIndex_ >= 0) {
        InterpolateCamera(currentTime_);
    }
}

void CameraEditor::AddKeyFrame(float timePoint) {
    if (currentAnimationIndex_ < 0) {
        return;
    }

    auto& animation = animations_[currentAnimationIndex_];

    CameraKeyFrame newKeyFrame;
    newKeyFrame.timePoint = timePoint;
    newKeyFrame.position  = viewProjection_->translation_;
    newKeyFrame.rotation  = viewProjection_->rotation_;
    newKeyFrame.fov       = viewProjection_->fovAngleY_;

    animation.keyFrames.push_back(newKeyFrame);
    SortKeyFramesByTime();
    RecalculateTotalDuration();
}

void CameraEditor::DeleteKeyFrame(int32_t index) {
    if (currentAnimationIndex_ < 0)
        return;

    auto& animation = animations_[currentAnimationIndex_];
    if (index >= 0 && index < animation.keyFrames.size()) {
        animation.keyFrames.erase(animation.keyFrames.begin() + index);
        RecalculateTotalDuration();
    }
}

void CameraEditor::UpdateKeyFrameFromCurrentCamera(int32_t index) {
    if (currentAnimationIndex_ < 0) {
        return;
    }

    auto& animation = animations_[currentAnimationIndex_];
    if (index >= 0 && index < animation.keyFrames.size()) {
        auto& keyFrame    = animation.keyFrames[index];
        keyFrame.position = viewProjection_->translation_;
        keyFrame.rotation = viewProjection_->rotation_;
        keyFrame.fov      = viewProjection_->fovAngleY_;
    }
}

void CameraEditor::ApplyKeyFrameToCamera(int32_t index) {
    if (currentAnimationIndex_ < 0) {
        return;
    }

    auto& animation = animations_[currentAnimationIndex_];
    if (index >= 0 && index < animation.keyFrames.size()) {
        auto& keyFrame                = animation.keyFrames[index];
        viewProjection_->translation_ = keyFrame.position;
        viewProjection_->rotation_    = keyFrame.rotation;
        viewProjection_->fovAngleY_   = keyFrame.fov;
        currentTime_                  = keyFrame.timePoint;
    }
}

void CameraEditor::CreateNewAnimation(const std::string& name) {
    std::string uniqueName = GenerateUniqueAnimationName(name);

    CameraAnimation newAnimation;
    newAnimation.name = uniqueName;
    animations_.push_back(newAnimation);
    currentAnimationName_  = uniqueName;
    currentAnimationIndex_ = static_cast<int32_t>(animations_.size() - 1);
}

void CameraEditor::DuplicateAnimation(const std::string& sourceName, const std::string& newName) {
    auto it = std::find_if(animations_.begin(), animations_.end(),
        [&sourceName](const CameraAnimation& anim) {
            return anim.name == sourceName;
        });

    if (it != animations_.end()) {
        CameraAnimation duplicated = *it;
        duplicated.name            = GenerateUniqueAnimationName(newName);
        animations_.push_back(duplicated);
        currentAnimationName_  = duplicated.name;
        currentAnimationIndex_ = static_cast<int32_t>(animations_.size() - 1);
    }
}

void CameraEditor::DeleteAnimation(const std::string& name) {
    auto it = std::find_if(animations_.begin(), animations_.end(),
        [&name](const CameraAnimation& anim) {
            return anim.name == name;
        });

    if (it != animations_.end()) {
        bool wasCurrent = (it->name == currentAnimationName_);
        animations_.erase(it);

        if (wasCurrent) {
            if (!animations_.empty()) {
                currentAnimationName_  = animations_[0].name;
                currentAnimationIndex_ = 0;
            } else {
                currentAnimationName_  = "";
                currentAnimationIndex_ = -1;
            }
            StopAnimation();
        }
    }
}

void CameraEditor::SaveAnimationsToJson(const std::string& filePath) {
    if (serializer_->SaveAnimationsToJson(animations_, filePath)) {
        std::cout << "Successfully saved animations to: " << filePath << std::endl;
    } else {
        std::cerr << "Failed to save animations: " << serializer_->GetLastErrorMessage() << std::endl;
    }
}

void CameraEditor::LoadAnimationsFromJson(const std::string& filePath) {
    std::vector<CameraAnimation> loadedAnimations;
    if (serializer_->LoadAnimationsFromJson(filePath, loadedAnimations)) {
        animations_ = std::move(loadedAnimations);

        if (!animations_.empty()) {
            currentAnimationName_  = animations_[0].name;
            currentAnimationIndex_ = 0;
        } else {
            currentAnimationName_  = "";
            currentAnimationIndex_ = -1;
        }

        StopAnimation();
        std::cout << "Successfully loaded animations from: " << filePath << std::endl;
    } else {
        std::cerr << "Failed to load animations: " << serializer_->GetLastErrorMessage() << std::endl;
    }
}

void CameraEditor::ExportCurrentAnimationToJson(const std::string& filePath) {
    if (currentAnimationIndex_ < 0) {
        std::cerr << "No animation selected for export" << std::endl;
        return;
    }

    if (serializer_->ExportAnimationToJson(animations_[currentAnimationIndex_], filePath)) {
        std::cout << "Successfully exported animation to: " << filePath << std::endl;
    } else {
        std::cerr << "Failed to export animation: " << serializer_->GetLastErrorMessage() << std::endl;
    }
}

void CameraEditor::ImportAnimationFromJson(const std::string& filePath) {
    CameraAnimation importedAnimation;
    if (serializer_->ImportAnimationFromJson(filePath, importedAnimation)) {
        // 名前の重複チェック
        importedAnimation.name = GenerateUniqueAnimationName(importedAnimation.name);

        animations_.push_back(importedAnimation);
        currentAnimationName_  = importedAnimation.name;
        currentAnimationIndex_ = static_cast<int32_t>(animations_.size() - 1);

        std::cout << "Successfully imported animation: " << importedAnimation.name << " from: " << filePath << std::endl;
    } else {
        std::cerr << "Failed to import animation: " << serializer_->GetLastErrorMessage() << std::endl;
    }
}

void CameraEditor::SaveOriginalCameraParams() {
    if (viewProjection_) {
        originalCameraParams_ = *viewProjection_;
        hasOriginalParams_    = true;
    }
}

void CameraEditor::RestoreOriginalCameraParams() {
    if (hasOriginalParams_ && viewProjection_) {
        *viewProjection_ = originalCameraParams_;
    }
}

void CameraEditor::SortKeyFramesByTime() {
    if (currentAnimationIndex_ < 0)
        return;

    auto& animation = animations_[currentAnimationIndex_];
    std::sort(animation.keyFrames.begin(), animation.keyFrames.end(),
        [](const CameraKeyFrame& a, const CameraKeyFrame& b) {
            return a.timePoint < b.timePoint;
        });
}

void CameraEditor::RecalculateTotalDuration() {
    if (currentAnimationIndex_ < 0)
        return;

    auto& animation = animations_[currentAnimationIndex_];
    if (!animation.keyFrames.empty()) {
        animation.totalDuration = animation.keyFrames.back().timePoint;
    } else {
        animation.totalDuration = 0.0f;
    }
}

std::string CameraEditor::GenerateUniqueAnimationName(const std::string& baseName) {
    std::string uniqueName = baseName;
    int32_t counter        = 1;

    while (std::find_if(animations_.begin(), animations_.end(),
               [&uniqueName](const CameraAnimation& anim) {
                   return anim.name == uniqueName;
               })
           != animations_.end()) {
        uniqueName = baseName + "_" + std::to_string(counter);
        counter++;
    }

    return uniqueName;
}

int32_t CameraEditor::FindKeyFrameIndexAtTime(float time) {
    if (currentAnimationIndex_ < 0)
        return -1;

    auto& animation = animations_[currentAnimationIndex_];

    for (int32_t i = 0; i < animation.keyFrames.size(); ++i) {
        if (std::abs(animation.keyFrames[i].timePoint - time) < 0.01f) {
            return i;
        }
    }

    return -1;
}