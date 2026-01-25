#include "ShakeEditor.h"

using namespace KetaEngine;
#include <imgui.h>

void ShakeEditor::Init(const std::string& typeName) {
    BaseEffectEditor::Init(typeName);

    preViewObj_.reset(Object3d::CreateModel("debugCube.obj"));
    preViewObj_->SetIsDraw(false);
}

void ShakeEditor::Update(float speedRate) {
    // 基底クラスのUpdate呼び出し
    BaseEffectEditor::Update(speedRate);

    // プレビューオブジェクトを更新
    UpdatePreviewObject(speedRate);
}

void ShakeEditor::UpdatePreviewObject(float speedRate) {
    if (!preViewObj_) {
        return;
    }
    speedRate;
    preViewObj_->SetIsDraw(showPreview_);

    // ベース値をリセット
    preViewObj_->transform_.translation_ = basePos_;

    // 選択中のアニメーションを適用
    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {

            auto* shake = category.effects[category.selectedEffectIndex].get();

            if (shake && shake->IsPlaying()) {
                preViewObj_->transform_.translation_ = basePos_ + shake->GetShakeOffset();
            }
        }
    }

    preViewObj_->Update();
}

void ShakeEditor::EditorUpdate() {
    BaseEffectEditor::EditorUpdate();
}

void ShakeEditor::PlaySelectedAnimation() {
    auto* selectedShake = GetSelectedEffect();
    if (selectedShake) {
        selectedShake->Play();
    }
}

std::unique_ptr<ShakeData> ShakeEditor::CreateEffectData() {
    return std::make_unique<ShakeData>();
}

void ShakeEditor::RenderSpecificUI() {
    ImGui::SeparatorText("Shake Editor Settings");

    ImGui::Checkbox("Show Preview Object", &showPreview_);
    ImGui::DragFloat3("Base Position", &basePos_.x, 0.1f);

    ImGui::Separator();
    BaseEffectEditor::RenderPlayBack();
}

std::string ShakeEditor::GetFolderName() const {
    return "ShakeEditor/";
}
