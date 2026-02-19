#include "RailEditor.h"

using namespace KetaEngine;
#include "3D/Line3D/Line3D.h"
#include <imgui.h>

RailEditor::RailEditor()  = default;
RailEditor::~RailEditor() = default;

void RailEditor::Init(const std::string& editorName) {
    BaseEffectEditor::Init(editorName);

    preViewObj_.reset(Object3d::CreateModel("TestObj/DebugCube.obj"));
    preViewObj_->SetIsDraw(false);
    debugLine3D_.reset(Line3D::Create(1000));
}

void RailEditor::Update(float speedRate) {
    // 全カテゴリーの全レールを更新
    for (auto& category : categories_) {
        for (auto& rail : category.effects) {
            rail->Update(speedRate);

            if (preViewObj_ && selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
                auto& selectedCategory = categories_[selectedCategoryIndex_];
                if (selectedCategory.selectedEffectIndex >= 0 && selectedCategory.selectedEffectIndex < static_cast<int>(selectedCategory.effects.size())) {
                    if (rail.get() == selectedCategory.effects[selectedCategory.selectedEffectIndex].get()) {
                        preViewObj_->transform_.translation_ = basePos_ + rail->GetCurrentPosition();
                    }
                }
            }
        }
    }

    preViewObj_->SetIsDraw(isPreViewDraw_);
    DebugLineSet();
}

void RailEditor::DebugLineSet() {
    if (debugLine3D_) {
        debugLine3D_->Reset();

        if (selectedCategoryIndex_ < 0 || selectedCategoryIndex_ >= static_cast<int>(categories_.size())) {
            return;
        }

        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex < 0 || category.selectedEffectIndex >= static_cast<int>(category.effects.size())) {
            return;
        }

        Vector4 lineColor = Vector4(1.0f, 1.0f, 0.0f, 1.0f);

        if (category.effects[category.selectedEffectIndex]->IsPlaying()) {
            lineColor = Vector4::kGREEN();
        }

        category.effects[category.selectedEffectIndex]->SetControlPointLines(debugLine3D_.get(), lineColor);
    }
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

void RailEditor::RenderSpecificUI() {
    ImGui::SeparatorText("Rail Editor Settings");

    ImGui::Checkbox("Show Preview Object", &isPreViewDraw_);
    ImGui::DragFloat3("Base Position", &basePos_.x, 0.1f);

    if (selectedCategoryIndex_ >= 0 && selectedCategoryIndex_ < static_cast<int>(categories_.size())) {
        auto& category = categories_[selectedCategoryIndex_];
        if (category.selectedEffectIndex >= 0 && category.selectedEffectIndex < static_cast<int>(category.effects.size())) {
            ImGui::Separator();
            BaseEffectEditor::RenderPlayBack();
        }
    }
}

int RailEditor::GetRailCount() const {
    int count = 0;
    for (const auto& category : categories_) {
        count += static_cast<int>(category.effects.size());
    }
    return count;
}

std::string RailEditor::GetFolderName() const {
    return folderName_;
}
