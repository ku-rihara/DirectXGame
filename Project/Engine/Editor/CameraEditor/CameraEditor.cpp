#include "CameraEditor.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <filesystem>
#include <imgui.h>

void CameraEditor::Init(const std::string& animationName, const bool& isUseCategory) {
    BaseEffectEditor::Init(animationName, isUseCategory);

    preViewCameraObj_.reset(Object3d::CreateModel("debugCube.obj"));
    preViewFollowObj_.reset(Object3d::CreateModel("debugCube.obj"));

    preViewCameraObj_->SetIsDraw(false);
    preViewFollowObj_->SetIsDraw(false);
}

void CameraEditor::AddAnimation(const std::string& animationName) {
    BaseEffectEditor::AddEffect(animationName);
}

void CameraEditor::Update(const float& speedRate) {
    // 未使用
    speedRate;

    for (auto& cAnime : effects_) {
        cAnime->Update();
    }

    // 自動でViewProjectionに適用
    if (autoApplyToViewProjection_ && viewProjection_) {
        ApplyToViewProjection();
    }

    // debugObjectの更新
    if (preViewCameraObj_ && viewProjection_) {
        preViewCameraObj_->transform_.translation_ = preViewFollowObj_->transform_.translation_ + viewProjection_->translation_ + viewProjection_->positionOffset_;
        preViewCameraObj_->transform_.rotation_    = preViewFollowObj_->transform_.rotation_ + viewProjection_->rotation_ + viewProjection_->rotationOffset_;
    }

    preViewFollowObj_->SetIsDraw(isPreViewDraw_);
    preViewCameraObj_->SetIsDraw(isPreViewDraw_);
}

void CameraEditor::PlaySelectedAnimation() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        // ViewProjectionの初期値を保存
        if (viewProjection_) {
            effects_[selectedIndex_]->SetInitialValues(
                viewProjection_->positionOffset_,
                viewProjection_->rotationOffset_,
                viewProjection_->fovAngleY_);
        }
        effects_[selectedIndex_]->Play();
    }
}

void CameraEditor::ApplyToViewProjection() {
    if (!viewProjection_) {
        return;
    }

    if (keyFramePreviewMode_) {
        return;
    }

    // 再生中のアニメーションがあれば適用
    for (auto& animation : effects_) {
        animation->ApplyToViewProjection(*viewProjection_);
    }
}

void CameraEditor::ApplySelectedKeyFrameToViewProjection() {
    if (!viewProjection_ || selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(effects_.size())) {
        return;
    }

    auto* selectedAnim     = effects_[selectedIndex_].get();
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
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        return effects_[selectedIndex_].get();
    }
    return nullptr;
}

void CameraEditor::SetViewProjection(ViewProjection* vp) {
    viewProjection_ = vp;
}

std::unique_ptr<CameraAnimationData> CameraEditor::CreateEffectData() {
    return std::make_unique<CameraAnimationData>();
}

void CameraEditor::RenderSpecificUI() {
    // CameraEditor固有のUI表示
    ImGui::SeparatorText("Camera Editor Settings");

    ImGui::Checkbox("Auto Apply to ViewProjection", &autoApplyToViewProjection_);
    ImGui::Checkbox("KeyFrame Preview Mode", &keyFramePreviewMode_);
    ImGui::Checkbox("Show Preview Objects", &isPreViewDraw_);

    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        ImGui::Separator();

        if (ImGui::Button("Play")) {
            PlaySelectedAnimation();
        }
        ImGui::SameLine();

        if (ImGui::Button("Pause")) {
            PauseSelectedAnimation();
        }
        ImGui::SameLine();

        if (ImGui::Button("Reset")) {
            ResetSelectedAnimation();
        }

        if (keyFramePreviewMode_ && ImGui::Button("Apply Selected KeyFrame to ViewProjection")) {
            ApplySelectedKeyFrameToViewProjection();
        }
    }
}

void CameraEditor::EditorUpdate() {
    BaseEffectEditor::EditorUpdate();
}

std::string CameraEditor::GetFolderPath() const {
    return folderName_;
}

void CameraEditor::PauseSelectedAnimation() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        effects_[selectedIndex_]->Pause();
    }
}

void CameraEditor::ResetSelectedAnimation() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        effects_[selectedIndex_]->Reset();
    }
}

bool CameraEditor::IsSelectedAnimationPlaying() const {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        return effects_[selectedIndex_]->IsPlaying();
    }
    return false;
}

bool CameraEditor::IsSelectedAnimationFinished() const {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        return effects_[selectedIndex_]->IsFinished();
    }
    return false;
}