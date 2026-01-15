#include "DissolveEditor.h"

using namespace KetaEngine;
#include <imgui.h>

void DissolveEditor::Init(const std::string& name, bool isUseCategory) {
    BaseEffectEditor::Init(name, isUseCategory);
    preViewObj_.reset(Object3d::CreateModel("DebugCube.obj"));
    preViewObj_->SetIsDraw(false);
}

std::unique_ptr<DissolveData> DissolveEditor::CreateEffectData() {
    return std::make_unique<DissolveData>();
}

void DissolveEditor::RenderSpecificUI() {
    // プレビュー用の更新処理
    if (preViewObj_) {
        auto* selectedDissolve = GetSelectedEffect();
        if (selectedDissolve && selectedDissolve->IsPlaying()) {
            preViewObj_->GetModelMaterial()->GetMaterialData()->dissolveThreshold = selectedDissolve->GetCurrentThreshold();
            preViewObj_->GetModelMaterial()->GetMaterialData()->enableDissolve    = selectedDissolve->IsDissolveEnabled();

            // テクスチャも更新
            if (!selectedDissolve->GetCurrentTexturePath().empty()) {
                preViewObj_->GetModelMaterial()->SetDissolveNoizeTexture(selectedDissolve->GetCurrentTexturePath());
            }
        }
    }

    // プレビューコントロール
    ImGui::SeparatorText("Preview Control");
    if (preViewObj_) {
        ImGui::Text("Preview Object: Active");
        static bool isDraw = false;
        ImGui::Checkbox("Is Draw", &isDraw);
        preViewObj_->SetIsDraw(isDraw);
        ImGui::DragFloat3("Position", &preViewObj_->transform_.translation_.x, 0.1f);

        static float manualThreshold = 1.0f;
        static bool manualEnable     = false;

        preViewObj_->GetModelMaterial()->GetMaterialData()->dissolveThreshold = manualThreshold;
        preViewObj_->GetModelMaterial()->GetMaterialData()->enableDissolve    = manualEnable;

        ImGui::Checkbox("Manual Enable", &manualEnable);
        ImGui::DragFloat("Manual Threshold", &manualThreshold, 0.01f, 0.0f, 1.0f);
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

std::string DissolveEditor::GetFolderPath() const {
    return "DissolveEditor/";
}