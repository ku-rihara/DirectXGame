#include "CameraEditor.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "MathFunction.h"
#include "Matrix4x4.h"
#include <filesystem>
#include <imgui.h>
#include <numbers>

void CameraEditor::Init(const std::string& animationName) {
    BaseEffectEditor::Init(animationName);

    preViewCameraObj_.reset(Object3d::CreateModel("debugCube.obj"));
    preViewFollowObj_.reset(Object3d::CreateModel("debugCube.obj"));

    preViewCameraObj_->SetIsDraw(false);
    preViewFollowObj_->SetIsDraw(false);
}

void CameraEditor::Update(float speedRate) {
    if (!viewProjection_) {
        return;
    }

    BaseEffectEditor::Update(speedRate);

    // 自動でViewProjectionに適用
    if (autoApplyToViewProjection_) {
        ApplyToViewProjection();
    } else if (keyFramePreviewMode_) {
        ApplySelectedKeyFrameToViewProjection();
    }

    // debugObjectの更新
    if (preViewCameraObj_) {
        preViewCameraObj_->transform_.translation_ = preViewFollowObj_->transform_.translation_ + viewProjection_->translation_ + viewProjection_->positionOffset_;
        preViewCameraObj_->transform_.rotation_    = preViewFollowObj_->transform_.rotation_ + viewProjection_->rotation_ + viewProjection_->rotationOffset_;
    }

    preViewFollowObj_->SetIsDraw(isPreViewDraw_);
    preViewCameraObj_->SetIsDraw(isPreViewDraw_);
}

void CameraEditor::PlaySelectedAnimation() {
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {
            auto* animation = category.effects[category.selectedEffectIndex].get();

            // ViewProjectionの初期値を保存
            if (viewProjection_) {
                animation->SetInitialValues(
                    viewProjection_->positionOffset_,
                    viewProjection_->rotationOffset_,
                    viewProjection_->fovAngleY_);
            }

            // 注視点ターゲットを設定
            if (preViewFollowObj_) {
                animation->SetLookAtTarget(&preViewFollowObj_->transform_);
            }

            animation->Play();
        }
    }
}

void CameraEditor::ApplyToViewProjection() {
    if (!viewProjection_) {
        return;
    }

    if (keyFramePreviewMode_) {
        return;
    }

    // 全カテゴリーの全アニメーションに適用
    for (auto& category : categories_) {
        for (auto& animation : category.effects) {
            animation->ApplyToViewProjection(*viewProjection_);
        }
    }
}

void CameraEditor::ApplySelectedKeyFrameToViewProjection() {
    if (!viewProjection_ || selectedCategoryIndex_ < 0 || selectedCategoryIndex_ >= static_cast<int>(categories_.size())) {
        return;
    }

    auto& category = categories_[selectedCategoryIndex_];
    if (category.selectedEffectIndex < 0 || category.selectedEffectIndex >= static_cast<int>(category.effects.size())) {
        return;
    }

    auto* selectedAnime    = category.effects[category.selectedEffectIndex].get();
    auto* selectedKeyFrame = selectedAnime->GetSelectedKeyFrame();

    if (!selectedKeyFrame) {
        return;
    }

    // 注視点モードかどうかをチェック
    if (selectedAnime->IsUseLookAt()) {
        // 注視点モードの場合
        Vector3 lookAtTargetPos = preViewFollowObj_->transform_.translation_;
        Vector3 animeRotation   = selectedKeyFrame->GetEditRotation();
        Vector3 posOffset       = selectedKeyFrame->GetEditPosition();

        viewProjection_->ApplyLookAtMode(lookAtTargetPos, animeRotation, posOffset);

        // FOVを適用
        viewProjection_->fovAngleY_ = selectedKeyFrame->GetEditFov();

    } else {
        // 通常モードの場合
        viewProjection_->positionOffset_ = selectedKeyFrame->GetEditPosition();
        viewProjection_->rotationOffset_ = selectedKeyFrame->GetEditRotation();
        viewProjection_->fovAngleY_      = selectedKeyFrame->GetEditFov();
    }
}

CameraAnimationData* CameraEditor::GetSelectedAnimation() {
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {
            return category.effects[category.selectedEffectIndex].get();
        }
    }
    return nullptr;
}

void CameraEditor::SetViewProjection(ViewProjection* vp) {
    viewProjection_ = vp;
}

void CameraEditor::SetLookAtTarget() {
    // 全カテゴリーの全アニメーションにターゲットを設定
    for (auto& category : categories_) {
        for (auto& animation : category.effects) {
            animation->SetLookAtTarget(&preViewFollowObj_->transform_);
        }
    }
}

std::unique_ptr<CameraAnimationData> CameraEditor::CreateEffectData() {
    auto data = std::make_unique<CameraAnimationData>();

    // 注視点ターゲットを設定
    if (preViewFollowObj_) {
        data->SetLookAtTarget(&preViewFollowObj_->transform_);
    }

    return data;
}

void CameraEditor::RenderSpecificUI() {
    // CameraEditor固有のUI表示
    ImGui::SeparatorText("Camera Editor Settings");

    ImGui::Checkbox("Auto Apply to ViewProjection", &autoApplyToViewProjection_);
    ImGui::Checkbox("KeyFrame Preview Mode", &keyFramePreviewMode_);
    ImGui::Checkbox("Show Preview Objects", &isPreViewDraw_);

    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {
            ImGui::Separator();
            BaseEffectEditor::RenderPlayBack();
        }
    }
}

std::string CameraEditor::GetFolderName() const {
    return folderName_;
}

void CameraEditor::SetPreviewObjPos(const Vector3& pos) {
    preViewFollowObj_->transform_.translation_ = pos;
}