#include "ShakeEditor.h"

using namespace KetaEngine;
#include <imgui.h>

void ShakeEditor::Init(const std::string& editorName, bool isUseCategory) {
    BaseEffectEditor::Init(editorName, isUseCategory);

    preViewObj_.reset(Object3d::CreateModel("debugCube.obj"));
    preViewObj_->SetIsDraw(false);
}

void ShakeEditor::Update(float speedRate) {
    // すべてのシェイクを更新
    for (auto& shake : effects_) {
        shake->Update(speedRate);

        if (shake->IsPlaying() && preViewObj_) {
            preViewObj_->transform_.translation_ = basePos_ + shake->GetShakeOffset();
        }
    }

    preViewObj_->SetIsDraw(isPreViewDraw_);
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

std::string ShakeEditor::GetFolderPath() const {
    return folderName_;
}

void ShakeEditor::RenderSpecificUI() {
    ImGui::SeparatorText("Shake Editor Settings");

    ImGui::Checkbox("Show Preview Object", &isPreViewDraw_);
    ImGui::DragFloat3("Base Position", &basePos_.x, 0.1f);

    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        ImGui::Separator();
        BaseEffectEditor::RenderPlayBack();
    }
}
