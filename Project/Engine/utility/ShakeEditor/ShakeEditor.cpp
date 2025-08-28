#include "ShakeEditor.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void ShakeEditor::Init() {
    AllLoadFile();
    preViewObj_.reset(Object3d::CreateModel("DebugCube.obj"));
}

void ShakeEditor::AllLoadFile() {
    // ShakeEditorのShakeDataフォルダ内のすべてのファイルを検索
    std::string folderPath = "Resources/GlobalParameter/ShakeEditor/";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // 既存のシェイクをクリア
        shakes_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto shake = std::make_unique<ShakeData>();
                shake->Init(fileName);
                shake->LoadData(); //Load
                shakes_.push_back(std::move(shake));
            }
        }
    }
}

void ShakeEditor::Update(float deltaTime) {
    // すべてのシェイクを更新
    for (auto& shake : shakes_) {
        shake->Update(deltaTime);
        if (shake->IsPlaying() && preViewObj_) {
            preViewObj_->transform_.translation_=shake->GetShakeOffset();
    
        }
    }
}

void ShakeEditor::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Shake Manager")) {

        // 全体制御
        if (ImGui::Button("Load All Shakes")) {
            AllLoadFile();
            MessageBoxA(nullptr, "All shakes loaded successfully.", "Shake Editor", 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save All Shakes")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All shakes saved successfully.", "Shake Editor", 0);
        }

        ImGui::Separator();

        // 新規追加
        ImGui::InputText("New Shake Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add Shake")) {
            if (strlen(nameBuffer_) > 0) {
                AddShake(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        ImGui::Separator();

        // シェイクリスト表示
        ImGui::Text("Shakes (%d):", static_cast<int>(shakes_.size()));
        for (int i = 0; i < static_cast<int>(shakes_.size()); i++) {
            ImGui::PushID(i);

            bool isSelected       = (selectedIndex_ == i);
            std::string labelText = shakes_[i]->GetGroupName();

            // 再生中のシェイクを強調表示
            if (shakes_[i]->IsPlaying()) {
                labelText += " [PLAYING]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedIndex_ = i;
            }

            if (shakes_[i]->IsPlaying()) {
                ImGui::PopStyleColor();
            }

            

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択されたシェイクの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
            shakes_[selectedIndex_]->AdjustParam();
        }
    }
#endif
}

void ShakeEditor::AddShake(const std::string& shakeName) {
    auto shake = std::make_unique<ShakeData>();
    shake->Init(shakeName);
    shakes_.push_back(std::move(shake));
    selectedIndex_ = static_cast<int>(shakes_.size()) - 1;
}

void ShakeEditor::RemoveShake(int index) {
    if (index >= 0 && index < static_cast<int>(shakes_.size())) {
        shakes_.erase(shakes_.begin() + index);

        // 選択インデックス調整
        if (selectedIndex_ >= index) {
            selectedIndex_--;
            if (selectedIndex_ < 0 && !shakes_.empty()) {
                selectedIndex_ = 0;
            } else if (shakes_.empty()) {
                selectedIndex_ = -1;
            }
        }
    }
}

void ShakeEditor::AllSaveFile() {
    // すべてのシェイクデータを保存
    for (auto& shake : shakes_) {
        shake->SaveData();
    }
}

ShakeData* ShakeEditor::GetSelectedShake() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(shakes_.size())) {
        return shakes_[selectedIndex_].get();
    }
    return nullptr;
}

ShakeData* ShakeEditor::GetShakeByName(const std::string& name) {
    auto it = std::find_if(shakes_.begin(), shakes_.end(),
        [&name](const std::unique_ptr<ShakeData>& shake) {
            return shake->GetGroupName() == name;
        });

    if (it != shakes_.end()) {
        return it->get();
    }
    return nullptr;
}