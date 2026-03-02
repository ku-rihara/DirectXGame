#include "SpriteEaseAnimationEditor.h"

using namespace KetaEngine;
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imgui.h>

void SpriteEaseAnimationEditor::Init(const std::string& typeName) {
    BaseEffectEditor::Init(typeName);
    InitPreviewSprite();
}

void SpriteEaseAnimationEditor::InitPreviewSprite() {
    previewSprite_ = Sprite::Create(previewTextureNameBuffer_, false);

    if (previewSprite_) {
        previewSprite_->transform_.scale  = previewBaseTransform_.scale;
        previewSprite_->transform_.pos    = previewBaseTransform_.position;
        previewSprite_->transform_.rotate = previewBaseTransform_.rotation;
        previewSprite_->SetColor(previewBaseTransform_.color);
        previewSprite_->SetAlpha(previewBaseTransform_.alpha);
        previewSprite_->SetIsDraw(false);
    }
}

void SpriteEaseAnimationEditor::Update(float speedRate) {
    BaseEffectEditor::Update(speedRate);
    UpdatePreviewSprite();
}

void SpriteEaseAnimationEditor::UpdatePreviewSprite() {
    if (!previewSprite_) {
        return;
    }

    previewSprite_->SetIsDraw(showPreview_);

    // ベース値をリセット
    previewSprite_->transform_.scale  = previewBaseTransform_.scale;
    previewSprite_->transform_.pos    = previewBaseTransform_.position;
    previewSprite_->transform_.rotate = previewBaseTransform_.rotation;
    previewSprite_->SetColor(previewBaseTransform_.color);
    previewSprite_->SetAlpha(previewBaseTransform_.alpha);

    // 選択中のアニメーションを適用
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {

            auto* animation = category.effects[category.selectedEffectIndex].get();

            if (animation && animation->IsPlaying()) {
                Vector2 scaleOffset    = animation->GetActiveScale();
                Vector2 positionOffset = animation->GetActivePosition();
                Vector3 rotationOffset = animation->GetActiveRotation();
                Vector3 colorValue     = animation->GetActiveColor();
                float alphaValue       = animation->GetActiveAlpha();

                previewSprite_->transform_.scale.x  = previewBaseTransform_.scale.x * scaleOffset.x;
                previewSprite_->transform_.scale.y  = previewBaseTransform_.scale.y * scaleOffset.y;
                previewSprite_->transform_.pos.x    = previewBaseTransform_.position.x + positionOffset.x;
                previewSprite_->transform_.pos.y    = previewBaseTransform_.position.y + positionOffset.y;
                previewSprite_->transform_.rotate    = previewBaseTransform_.rotation + rotationOffset;
                previewSprite_->SetColor(colorValue);
                previewSprite_->SetAlpha(alphaValue);
            }
        }
    }
}

std::unique_ptr<SpriteEaseAnimationData> SpriteEaseAnimationEditor::CreateEffectData() {
    return std::make_unique<SpriteEaseAnimationData>();
}

void SpriteEaseAnimationEditor::RenderSpecificUI() {
    // プレビュー設定
    ImGui::SeparatorText("Preview Settings");
    ImGui::Checkbox("Show Preview", &showPreview_);

    if (showPreview_) {
        ImGui::InputText("Preview Texture", previewTextureNameBuffer_, IM_ARRAYSIZE(previewTextureNameBuffer_));

        if (ImGui::Button("Change Texture")) {
            ChangePreviewTexture(previewTextureNameBuffer_);
        }

        ImGui::DragFloat2("Base Scale", &previewBaseTransform_.scale.x, 0.01f);
        ImGui::DragFloat2("Base Position", &previewBaseTransform_.position.x, 0.1f);
        ImGui::DragFloat3("Base Rotation", &previewBaseTransform_.rotation.x, 0.01f);
        ImGui::ColorEdit3("Base Color", &previewBaseTransform_.color.x);
        ImGui::SliderFloat("Base Alpha", &previewBaseTransform_.alpha, 0.0f, 1.0f);

        if (ImGui::Button("Reset Preview Transform")) {
            previewBaseTransform_.scale    = Vector2::OneVector();
            previewBaseTransform_.position = Vector2::ZeroVector();
            previewBaseTransform_.rotation = Vector3::ZeroVector();
            previewBaseTransform_.color    = {1.0f, 1.0f, 1.0f};
            previewBaseTransform_.alpha    = 1.0f;
        }
    }

    // 再生コントロール
    ImGui::SeparatorText("Animation Control");

    BaseEffectEditor::RenderPlayBack();
}

void SpriteEaseAnimationEditor::ChangePreviewTexture(const std::string& textureName) {
    if (previewSprite_) {
        delete previewSprite_;
        previewSprite_ = nullptr;
    }
    previewSprite_ = Sprite::Create(textureName, false);
    if (previewSprite_) {
        previewSprite_->SetIsDraw(showPreview_);
    }
}

void SpriteEaseAnimationEditor::PlaySelectedAnimation() {
    BaseEffectEditor::PlaySelectedAnimation();
}

std::string SpriteEaseAnimationEditor::GetFolderName() const {
    return folderName_;
}
