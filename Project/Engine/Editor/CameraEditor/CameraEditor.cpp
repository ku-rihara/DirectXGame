#include "CameraEditor.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "MathFunction.h"
#include "Matrix4x4.h"
#include <filesystem>
#include <imgui.h>
#include <numbers>

void CameraEditor::Init(const std::string& animationName, bool isUseCategory) {
    BaseEffectEditor::Init(animationName, isUseCategory);

    preViewCameraObj_.reset(Object3d::CreateModel("debugCube.obj"));
    preViewFollowObj_.reset(Object3d::CreateModel("debugCube.obj"));

    preViewCameraObj_->SetIsDraw(false);
    preViewFollowObj_->SetIsDraw(false);
}

void CameraEditor::Update(float speedRate) {
    // 未使用
    speedRate;

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
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        // ViewProjectionの初期値を保存
        if (viewProjection_) {
            effects_[selectedIndex_]->SetInitialValues(
                viewProjection_->positionOffset_,
                viewProjection_->rotationOffset_,
                viewProjection_->fovAngleY_);
        }

        // 注視点ターゲットを設定
        if (preViewFollowObj_) {
            effects_[selectedIndex_]->SetLookAtTarget(&preViewFollowObj_->transform_);
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

    // 注視点モードかどうかをチェック
    if (selectedAnim->IsUseLookAt()) {
        // 注視点モードの場合
        Vector3 lookAtTargetPos = preViewFollowObj_->transform_.translation_;
        Vector3 animRotation    = selectedKeyFrame->GetEditRotation();
        float distance          = selectedAnim->GetLookAtTarget().Length(); 

        // Y軸回転行列を作成
        Matrix4x4 rotateY = MakeRotateYMatrix(animRotation.y);

        // X軸回転を作成
        Matrix4x4 rotateX = MakeRotateXMatrix(-animRotation.x);

        // 回転を合成
        Matrix4x4 rotateMatrix = rotateY * rotateX;

        // 基本距離ベクトル
        Vector3 distanceVec = {0.0f, 0.0f, distance};

        // アニメーションからの位置オフセット
        Vector3 posOffset = selectedKeyFrame->GetEditPosition();

        // 回転を適用してカメラオフセット位置を計算
        Vector3 cameraOffset = TransformNormal(distanceVec + posOffset, rotateMatrix);

        // 注視点からのオフセットでカメラ位置を決定
        Vector3 cameraPos             = lookAtTargetPos + cameraOffset;
        viewProjection_->translation_ = cameraPos;

        // カメラから注視点への方向ベクトルを計算
        Vector3 toTarget = lookAtTargetPos - cameraPos;

        if (toTarget.Length() > 0.001f) {
            toTarget = toTarget.Normalize();

            // Y軸回転（水平方向）を計算
            float actualYaw = std::atan2(toTarget.x, toTarget.z);

            // X軸回転（垂直方向）を計算
            float horizontalLength = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
            float actualPitch      = std::atan2(-toTarget.y, horizontalLength);

            // 計算した回転を適用
            viewProjection_->rotation_.y = actualYaw;
            viewProjection_->rotation_.x = actualPitch;
            viewProjection_->rotation_.z = 0.0f;
        }

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
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        return effects_[selectedIndex_].get();
    }
    return nullptr;
}

void CameraEditor::SetViewProjection(ViewProjection* vp) {
    viewProjection_ = vp;
}

void CameraEditor::SetLookAtTarget() {
  
    // 既存の全アニメーションにもターゲットを設定
    for (auto& animation : effects_) {
        animation->SetLookAtTarget(&preViewFollowObj_->transform_);
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


    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        ImGui::Separator();

        BaseEffectEditor::RenderPlayBack();
    }
}

void CameraEditor::EditorUpdate() {
    BaseEffectEditor::EditorUpdate();
}

std::string CameraEditor::GetFolderPath() const {
    return folderName_;
}