#include "DissolveEditor.h"

using namespace KetaEngine;
#include <imgui.h>

void DissolveEditor::Init(const std::string& typeName) {
    BaseEffectEditor::Init(typeName);
    preViewObj_.reset(Object3d::CreateModel("TestObj/DebugCube.obj"));
    preViewObj_->SetIsDraw(false);
}

std::unique_ptr<DissolveData> DissolveEditor::CreateEffectData() {
    return std::make_unique<DissolveData>();
}

void DissolveEditor::RenderSpecificUI() {
    // プレビューコントロール
    ImGui::SeparatorText("Preview Control");
    if (preViewObj_) {
        ImGui::Text("Preview Object: Active");
        ImGui::Checkbox("Is Draw", &showPreview_);
        preViewObj_->SetIsDraw(showPreview_);
        ImGui::DragFloat3("Position", &preViewObj_->transform_.translation_.x, 0.1f);

        auto* selectedDissolve = GetSelectedEffect();
        if (selectedDissolve && selectedDissolve->IsPlaying()) {
            // アニメーション再生中はDissolveDataの値を適用
            preViewObj_->GetModelMaterial()->GetMaterialData()->dissolveThreshold = selectedDissolve->GetCurrentThreshold();
            preViewObj_->GetModelMaterial()->GetMaterialData()->enableDissolve    = selectedDissolve->IsDissolveEnabled();

            if (!selectedDissolve->GetCurrentTexturePath().empty()) {
                preViewObj_->GetModelMaterial()->SetDissolveNoizeTexture(selectedDissolve->GetCurrentTexturePath());
            }
        } else {
            // 停止中は手動値を適用
            preViewObj_->GetModelMaterial()->GetMaterialData()->dissolveThreshold = manualThreshold_;
            preViewObj_->GetModelMaterial()->GetMaterialData()->enableDissolve    = manualEnable_ ? 1 : 0;

            if (selectedDissolve && !selectedDissolve->GetCurrentTexturePath().empty()) {
                preViewObj_->GetModelMaterial()->SetDissolveNoizeTexture(selectedDissolve->GetCurrentTexturePath());
            }
        }

        ImGui::Checkbox("Manual Enable", &manualEnable_);
        ImGui::DragFloat("Manual Threshold", &manualThreshold_, 0.01f, 0.0f, 1.0f);
    } else {
        ImGui::Text("Preview Object: None");
    }
}

void DissolveEditor::PlaySelectedAnimation() {
    auto* selected = GetSelectedEffect();
    if (selected) {
        selected->Play();
    }
}

std::string DissolveEditor::GetFolderName() const {
    return folderName_;
}
