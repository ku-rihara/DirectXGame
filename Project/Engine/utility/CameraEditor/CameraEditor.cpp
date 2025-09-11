#include "CameraEditor.h"
#include "Frame/Frame.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void CameraEditor::Init(ViewProjection* vp) {
    AllLoadFile();
    SetViewProjection(vp);
    debugObject_.reset(Object3d::CreateModel("debugCube.obj"));
    debugObject_->SetIsDraw(false);
}

void CameraEditor::AllLoadFile() {
    // CameraAnimationのAnimationDataフォルダ内のすべてのファイルを検索
    std::string folderPath = "Resources/GlobalParameter/CameraAnimation/AnimationData/";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // 既存のアニメーションをクリア
        animations_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto anim = std::make_unique<CameraAnimationData>();
                anim->Init(fileName);
                anim->LoadData(); //Load
                animations_.push_back(std::move(anim));
            }
        }
    }
}

void CameraEditor::AllSaveFile() {
    // すべてのアニメーションデータを保存
    for (auto& animation : animations_) {
        animation->SaveData();
    }
}

void CameraEditor::AddAnimation(const std::string& animationName) {
    auto anime = std::make_unique<CameraAnimationData>();
    anime->Init(animationName);
    animations_.push_back(std::move(anime));
}

void CameraEditor::Update(float deltaTime) {
    for (auto& cAnime : animations_) {
        cAnime->Update(deltaTime);
    }

    // 自動でViewProjectionに適用
    if (autoApplyToViewProjection_ && viewProjection_) {
        ApplyToViewProjection();
    }

    // debugObjectの更新
    if (debugObject_ && viewProjection_) {
        debugObject_->transform_.translation_ = viewProjection_->translation_ + viewProjection_->positionOffset_;
        debugObject_->transform_.rotation_    = viewProjection_->rotation_ + viewProjection_->rotationOffset_;
    }
}

void CameraEditor::PlaySelectedAnimation() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
        // ViewProjectionの初期値を保存
        if (viewProjection_) {
            animations_[selectedIndex_]->SetInitialValues(
                viewProjection_->positionOffset_,
                viewProjection_->rotationOffset_,
                viewProjection_->fovAngleY_);
        }
        animations_[selectedIndex_]->Play();
    }
}

void CameraEditor::PauseSelectedAnimation() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
        animations_[selectedIndex_]->Pause();
    }
}

void CameraEditor::ResetSelectedAnimation() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
        animations_[selectedIndex_]->Reset();
    }
}

bool CameraEditor::IsSelectedAnimationPlaying() const {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
        return animations_[selectedIndex_]->IsPlaying();
    }
    return false;
}

bool CameraEditor::IsSelectedAnimationFinished() const {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
        return animations_[selectedIndex_]->IsFinished();
    }
    return false;
}

void CameraEditor::ApplyToViewProjection() {
    if (!viewProjection_) {
        return;
    }

    if (keyFramePreviewMode_) {
        return;
    }

    // 再生中のアニメーションがあれば適用
    for (auto& animation : animations_) {
        animation->ApplyToViewProjection(*viewProjection_);
    }
}

void CameraEditor::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Camera Animation Manager")) {

        // ロード
        if (ImGui::Button("Load All Animations")) {
            AllLoadFile();
            MessageBoxA(nullptr, "All animations loaded successfully.", "Camera Animation", 0);
        }
        ImGui::SameLine();
        // セーブ
        if (ImGui::Button("Save All Animations")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All animations saved successfully.", "Camera Animation", 0);
        }

        ImGui::Separator();

       // 設定
        ImGui::Text("ViewProjection Mode:");
        if (ImGui::RadioButton("Auto Apply to ViewProjection", autoApplyToViewProjection_)) {
            autoApplyToViewProjection_ = true;
            keyFramePreviewMode_       = false;
         
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("KeyFrame Preview Mode", keyFramePreviewMode_)) {
            keyFramePreviewMode_       = true;
            autoApplyToViewProjection_ = false;
          
        }

        ImGui::Separator();

        // UI
        ImGui::InputText("New Animation Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));

        if (ImGui::Button("Add Animation")) {
            if (strlen(nameBuffer_) > 0) {
                AddAnimation(nameBuffer_);
                nameBuffer_[0] = '\0'; // クリア
            }
        }

        ImGui::Separator();

        // 一覧表示
        ImGui::Text("Animations (%zu):", animations_.size());
        for (int i = 0; i < static_cast<int>(animations_.size()); i++) {
            ImGui::PushID(i);
            bool isSelected = (selectedIndex_ == i);
            bool isPlaying  = animations_[i]->IsPlaying();
            bool isFinished = animations_[i]->IsFinished();

            std::string displayName = animations_[i]->GetGroupName();
            if (isPlaying) {
                displayName += " [PLAYING]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Playing
            } else if (isFinished) {
                displayName += " [FINISHED]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); // Finish
            }

            if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                selectedIndex_ = i;
               
            }

            if (isPlaying || isFinished) {
                ImGui::PopStyleColor();
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択中アニメーションの制御
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
            ImGui::SeparatorText("Animation Controls");
            ImGui::PushID("selected_controls");

            // 状態表示
            ImGui::Text("Selected: %s", animations_[selectedIndex_]->GetGroupName().c_str());
            ImGui::Text("Playing: %s", IsSelectedAnimationPlaying() ? "Yes" : "No");
            ImGui::Text("Finished: %s", IsSelectedAnimationFinished() ? "Yes" : "No");

            // KeyFrameプレビューモードの制御
            if (keyFramePreviewMode_) {
                ImGui::Separator();
                ImGui::SeparatorText("KeyFrame Preview");

                auto* selectedAnim        = animations_[selectedIndex_].get();
                int selectedKeyFrameIndex = selectedAnim->GetSelectedKeyFrameIndex();

                ImGui::Text("Selected KeyFrame: %d", selectedKeyFrameIndex);

                ApplySelectedKeyFrameToViewProjection();
            }

            ImGui::PopID();
            ImGui::Separator();

            // 選択中アニメーションの詳細編集
            animations_[selectedIndex_]->AdjustParam();
        }
    }
#endif
}

void CameraEditor::ApplySelectedKeyFrameToViewProjection() {
    if (!viewProjection_ || selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(animations_.size())) {
        return;
    }

    auto* selectedAnim     = animations_[selectedIndex_].get();
    auto* selectedKeyFrame = selectedAnim->GetSelectedKeyFrame();

    if (!selectedKeyFrame) {
        return;
    }

    // 選択中のKeyFrameの値をViewProjectionに適用
    viewProjection_->positionOffset_ = selectedKeyFrame->GetEditPosition();
    viewProjection_->rotationOffset_ = selectedKeyFrame->GetEditRotation();
    viewProjection_->fovAngleY_      = selectedKeyFrame->GetEditFov();
}

CameraAnimationData* CameraEditor::GetSelectedAnimation() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
        return animations_[selectedIndex_].get();
    }
    return nullptr;
}

void CameraEditor::SetViewProjection(ViewProjection* vp) {
    viewProjection_ = vp;
}