#include "ObjEaseAnimationEditor.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>
#include <Windows.h>

void ObjEaseAnimationEditor::Init(const std::string& typeName, const bool& isUseCategory) {
    BaseEffectEditor::Init(typeName, isUseCategory);
    InitPreviewObject();

}

void ObjEaseAnimationEditor::InitPreviewObject() {
    previewObject_.reset(Object3d::CreateModel("DebugCube.obj"));

    if (previewObject_) {
        previewObject_->SetIsAutoUpdate(false);
        previewObject_->transform_.scale_       = previewBaseTransform_.scale;
        previewObject_->transform_.rotation_    = previewBaseTransform_.rotation;
        previewObject_->transform_.translation_ = previewBaseTransform_.translation;
    }
    previewObject_->SetIsDraw(false);
}

void ObjEaseAnimationEditor::Update(float speedRate) {
    // 基底クラスのUpdate呼び出し
    BaseEffectEditor::Update(speedRate);

    // プレビューオブジェクトを更新
    UpdatePreviewObject();
}

void ObjEaseAnimationEditor::UpdatePreviewObject() {
    if (!previewObject_) {
        return;
    }

    previewObject_->SetIsDraw(showPreview_);

    // ベース値をリセット
    previewObject_->transform_.scale_       = previewBaseTransform_.scale;
    previewObject_->transform_.rotation_    = previewBaseTransform_.rotation;
    previewObject_->transform_.translation_ = previewBaseTransform_.translation;

    // 選択中のアニメーションを適用
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {

            auto* animation = category.effects[category.selectedEffectIndex].get();

            if (animation && animation->IsPlaying()) {
                Vector3 scaleOffset = animation->GetActiveKeyFrameValue(
                    ObjEaseAnimationData::TransformType::Scale);
                Vector3 rotationOffset = animation->GetActiveKeyFrameValue(
                    ObjEaseAnimationData::TransformType::Rotation);
                Vector3 translationOffset = animation->GetActiveKeyFrameValue(
                    ObjEaseAnimationData::TransformType::Translation);

                previewObject_->transform_.scale_       = previewBaseTransform_.scale * scaleOffset;
                previewObject_->transform_.rotation_    = previewBaseTransform_.rotation + rotationOffset;
                previewObject_->transform_.translation_ = previewBaseTransform_.translation + translationOffset;
            }
        }
    }

    previewObject_->Update();
}

std::unique_ptr<ObjEaseAnimationData> ObjEaseAnimationEditor::CreateEffectData() {
    return std::make_unique<ObjEaseAnimationData>();
}

void ObjEaseAnimationEditor::RenderSpecificUI() {
    // プレビュー設定
    ImGui::SeparatorText("Preview Settings");
    ImGui::Checkbox("Show Preview", &showPreview_);

    if (showPreview_) {
        ImGui::InputText("Preview Model", previewModelNameBuffer_, IM_ARRAYSIZE(previewModelNameBuffer_));

        if (ImGui::Button("Change Model")) {
            ChangePreviewModel(previewModelNameBuffer_);
        }

        ImGui::DragFloat3("Base Scale", &previewBaseTransform_.scale.x, 0.01f);
        ImGui::DragFloat3("Base Rotation", &previewBaseTransform_.rotation.x, 0.01f);
        ImGui::DragFloat3("Base Translation", &previewBaseTransform_.translation.x, 0.1f);

        if (ImGui::Button("Reset Preview Transform")) {
            previewBaseTransform_.scale       = Vector3::OneVector();
            previewBaseTransform_.rotation    = Vector3::ZeroVector();
            previewBaseTransform_.translation = Vector3::ZeroVector();
        }
    }

    // 再生コントロール
    ImGui::SeparatorText("Animation Control");

    BaseEffectEditor::RenderPlayBack();
}

std::string ObjEaseAnimationEditor::GetFolderPath() const {
    return animationFolderName_;
}

void ObjEaseAnimationEditor::ChangePreviewModel(const std::string& modelName) {
    previewObject_->SetModelByName(modelName);
}

void ObjEaseAnimationEditor::EditorUpdate() {
    BaseEffectEditor::EditorUpdate();
}

void ObjEaseAnimationEditor::PlaySelectedAnimation() {
    BaseEffectEditor::PlaySelectedAnimation();
}