#include "CameraEditor.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void CameraEditor::AllLoadFile() {
    // CameraAnimationのAnimationDataフォルダ内のすべてのファイルを検索
    std::string folderPath = "Resources/GlobalParameter/CameraAnimation/AnimationData/";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // 既存のアニメーションをクリア
        animations_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto anim = std::make_unique<CameraAnimationData>();
                anim->Init(fileName);
                anim->LoadData(); // アニメーションデータとキーフレームデータをすべてロード
                animations_.push_back(std::move(anim));
            }
        }
    }
}

void CameraEditor::AllSaveFile() {
    // すべてのアニメーションデータを保存
    for (auto& animation : animations_) {
        animation->SaveData();
    }
}

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

        // ロード
        if (ImGui::Button("Load All Animations")) {
            AllLoadFile();
            MessageBoxA(nullptr, "All animations loaded successfully.", "Camera Animation", 0);
        }
        ImGui::SameLine();
        // セーブ
        if (ImGui::Button("Save All Animations")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All animations saved successfully.", "Camera Animation", 0);
        }

        ImGui::Separator();

        // UI
        ImGui::InputText("New Animation Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));

        if (ImGui::Button("Add Animation")) {
            if (strlen(nameBuffer_) > 0) {
                AddAnimation(nameBuffer_);
                nameBuffer_[0] = '\0'; // クリア
            }
        }
        // 一覧表示
        ImGui::Separator();
        ImGui::Text("Animations (%zu):", animations_.size());
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