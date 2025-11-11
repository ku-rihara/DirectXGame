#include "RailEditor.h"
#include"Line3D/Line3d.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void RailEditor::Init() {
    AllLoadFile();
    preViewObj_.reset(Object3d::CreateModel("DebugCube.obj"));
    preViewObj_->SetIsDraw(false);
    debugLine3D_.reset(Line3D::Create(1000));
}

void RailEditor::AllLoadFile() {
    // RailEditorのRailDataフォルダ内のすべてのファイルを検索
    std::string folderPath = "Resources/GlobalParameter/RailEditor/Dates/";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // 既存のレールをクリア
        rails_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto rail = std::make_unique<RailData>();
                rail->Init(fileName);
                rail->LoadData();
                rails_.push_back(std::move(rail));
            }
        }
    }
}

void RailEditor::Update(const float& deltaTime) {
    // すべてのレールを更新
    for (auto& rail : rails_) {
        rail->Update(deltaTime, RailData::PositionMode::WORLD);
        if (rail->IsPlaying() && preViewObj_) {
            preViewObj_->transform_.translation_ = basePos_ + rail->GetCurrentPosition();
        }
    }

    preViewObj_->SetIsDraw(isPreViewDraw_);

    // ライン描画
    DebugLineSet();
}

void RailEditor::DebugLineSet() {
    // デバッグライン描画のリセット
    if (debugLine3D_) {
        debugLine3D_->Reset();

        // すべてのレールの制御点ラインを描画
        for (auto& rail : rails_) {
            Vector4 lineColor = Vector4(1.0f, 1.0f, 0.0f, 1.0f); // 黄色

            // 再生中のレールは緑色で描画
            if (rail->IsPlaying()) {
                lineColor = Vector4::kGREEN();
            }

            rail->SetControlPointLines(debugLine3D_.get(), lineColor);
        }
    }
}

void RailEditor::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Rail Manager")) {
        ImGui::PushID("Rail Manager");

        ImGui::Text("Preview Object:");
        ImGui::DragFloat3("preViewFollowObj", &basePos_.x, 0.1f);
        ImGui::Checkbox("IsDraw", &isPreViewDraw_);

        ImGui::Separator();
        ImGui::Text("Rail Edit:");

        // 新規追加
        ImGui::InputText("New Rail Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add Rail")) {
            if (strlen(nameBuffer_) > 0) {
                AddRail(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        ImGui::Separator();

        // レールリスト表示
        ImGui::Text("Rails (%d):", static_cast<int>(rails_.size()));
        for (int i = 0; i < static_cast<int>(rails_.size()); i++) {
            ImGui::PushID(i);

            bool isSelected       = (selectedIndex_ == i);
            std::string labelText = rails_[i]->GetGroupName();

            // 再生中のレールを強調表示
            if (rails_[i]->IsPlaying()) {
                labelText += " [PLAYING]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedIndex_ = i;
            }

            if (rails_[i]->IsPlaying()) {
                ImGui::PopStyleColor();
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択されたレールの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(rails_.size())) {
            rails_[selectedIndex_]->AdjustParam();
        }

        ImGui::Separator();
        ImGui::Text("File Operations:");

        // 選択中のレール個別のセーブ/ロード
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(rails_.size())) {
            ImGui::SeparatorText("Selected Rail File Operations");

            auto* selectedRail   = rails_[selectedIndex_].get();
            std::string railName = selectedRail->GetGroupName();

            // 個別Load
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
            if (ImGui::Button(("Load " + railName).c_str())) {
                selectedRail->LoadData();
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();

            // 個別Save
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
            if (ImGui::Button(("Save " + railName).c_str())) {
                selectedRail->SaveData();
                MessageBoxA(nullptr, (railName + " saved successfully.").c_str(), "Rail Editor", 0);
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::SeparatorText("All File Save/Load");

        // Load ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("Load All Rails")) {
            AllLoadFile();
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        // Save ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
        if (ImGui::Button("Save All Rails")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All Rails saved successfully.", "Rail Editor", 0);
        }
        ImGui::PopStyleColor(3);

        ImGui::PopID();
    }
#endif
}

void RailEditor::AddRail(const std::string& railName) {
    auto rail = std::make_unique<RailData>();
    rail->Init(railName);
    rails_.push_back(std::move(rail));
    selectedIndex_ = static_cast<int>(rails_.size()) - 1;
}

void RailEditor::RemoveRail(const int& index) {
    if (index >= 0 && index < static_cast<int>(rails_.size())) {
        rails_.erase(rails_.begin() + index);

        // 選択インデックス調整
        if (selectedIndex_ >= index) {
            selectedIndex_--;
            if (selectedIndex_ < 0 && !rails_.empty()) {
                selectedIndex_ = 0;
            } else if (rails_.empty()) {
                selectedIndex_ = -1;
            }
        }
    }
}

void RailEditor::AllSaveFile() {
    // すべてのレールデータを保存
    for (auto& rail : rails_) {
        rail->SaveData();
    }
}

RailData* RailEditor::GetSelectedRail() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(rails_.size())) {
        return rails_[selectedIndex_].get();
    }
    return nullptr;
}

RailData* RailEditor::GetRailByName(const std::string& name) {
    auto it = std::find_if(rails_.begin(), rails_.end(),
        [&name](const std::unique_ptr<RailData>& rail) {
            return rail->GetGroupName() == name;
        });

    if (it != rails_.end()) {
        return it->get();
    }
    return nullptr;
}