#include "CameraEditor.h"
#include <algorithm>
#include <imgui.h>

void CameraEditor::AddAnimation(const std::string& animationName) {
    auto anim = std::make_unique<CameraAnimationData>();
    anim->Init(animationName);
    animations_.push_back(std::move(anim));
}

void CameraEditor::Update(float deltaTime) {
    for (auto& cAnime : animations_) {
        cAnime->Update(deltaTime);
    }
}

void CameraEditor::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Camera Animation Manager")) {
      
        // UI
        ImGui::InputText("New Animation Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
          
        if (ImGui::Button("Add Animation")) {
            if (strlen(nameBuffer_) > 0) {
                AddAnimation(nameBuffer_);
                nameBuffer_[0] = '\0'; // クリア
            }
        }
        ImGui::Separator();

        // 一覧表示
        ImGui::Text("Animations:");
        for (int i = 0; i < static_cast<int>(animations_.size()); i++) {
            ImGui::PushID(i);
            bool isSelected = (selectedIndex_ == i);
            if (ImGui::Selectable(animations_[i]->GetGroupName().c_str(), isSelected)) {
                selectedIndex_ = i;
            }
            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択中アニメーションの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
            animations_[selectedIndex_]->AdjustParam();
        }
    }
#endif
}

CameraAnimationData* CameraEditor::GetSelectedAnimation() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(animations_.size())) {
        return animations_[selectedIndex_].get();
    }
    return nullptr;
}
