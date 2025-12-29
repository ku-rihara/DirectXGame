#include "RailEditor.h"

using namespace KetaEngine;
#include "Line3D/Line3d.h"
#include <imgui.h>

RailEditor::RailEditor()   = default;
RailEditor ::~RailEditor() = default;

void RailEditor::Init(const std::string& editorName, bool isUseCategory) {
    BaseEffectEditor::Init(editorName, isUseCategory);

    preViewObj_.reset(Object3d::CreateModel("DebugCube.obj"));
    preViewObj_->SetIsDraw(false);
    debugLine3D_.reset(Line3D::Create(1000));
}

void RailEditor::Update(float speedRate) {
    for (auto& rail : effects_) {
        rail->Update(speedRate);

        if ( preViewObj_) {
            preViewObj_->transform_.translation_ = basePos_ + rail->GetCurrentPosition();
        }
    }

    preViewObj_->SetIsDraw(isPreViewDraw_);
    DebugLineSet();
}

void RailEditor::DebugLineSet() {
    if (debugLine3D_) {
        debugLine3D_->Reset();

        for (auto& rail : effects_) {
            Vector4 lineColor = Vector4(1.0f, 1.0f, 0.0f, 1.0f);

            if (rail->IsPlaying()) {
                lineColor = Vector4::kGREEN();
            }

            rail->SetControlPointLines(debugLine3D_.get(), lineColor);
        }
    }
}

void RailEditor::EditorUpdate() {
    BaseEffectEditor::EditorUpdate();
}

void RailEditor::PlaySelectedAnimation() {
    auto* selectedRail = GetSelectedEffect();
    if (selectedRail) {
        selectedRail->Play();
    }
}

std::unique_ptr<RailData> RailEditor::CreateEffectData() {
    return std::make_unique<RailData>();
}

std::string RailEditor::GetFolderPath() const {
    return folderName_;
}

void RailEditor::RenderSpecificUI() {
    ImGui::SeparatorText("Rail Editor Settings");

    ImGui::Checkbox("Show Preview Object", &isPreViewDraw_);
    ImGui::DragFloat3("Base Position", &basePos_.x, 0.1f);

    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(effects_.size())) {
        ImGui::Separator();
        BaseEffectEditor::RenderPlayBack();
    }
}
