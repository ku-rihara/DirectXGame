#include "DissolveEditor.h"

using namespace KetaEngine;
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>
#include "Editor/ParameterEditor/GlobalParameter.h"

void DissolveEditor::Init() {
    AllLoadFile();
    preViewObj_.reset(Object3d::CreateModel("DebugCube.obj"));
}

void DissolveEditor::AllLoadFile() {
    // DissolveEditorのDissolveDataフォルダ内のすべてのファイルを検索
    std::string folderPath = GlobalParameter::GetInstance()->GetDirectoryPath() + "DissolveEditor/";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        // 既存のディゾルブをクリア
        dissolves_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto dissolve = std::make_unique<DissolveData>();
                dissolve->Init(fileName);
                dissolve->LoadData(); // Load
                dissolves_.push_back(std::move(dissolve));
            }
        }
    }
}

void DissolveEditor::Update(float deltaTime) {
    // すべてのディゾルブを更新
    for (auto& dissolve : dissolves_) {
        dissolve->Update(deltaTime);

        if (dissolve->IsPlaying() && preViewObj_) {

            preViewObj_->material_.SetDissolveThreshold(dissolve->GetCurrentThreshold());
            preViewObj_->material_.SetEnableDissolve(dissolve->IsDissolveEnabled());
        }
    }
}

void DissolveEditor::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Dissolve Manager")) {

        // 全体制御
        if (ImGui::Button("Load All Dissolves")) {
            AllLoadFile();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save All Dissolves")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All dissolves saved successfully.", "Dissolve Editor", 0);
        }

        ImGui::Separator();

        // 新規追加
        ImGui::InputText("New Dissolve Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add Dissolve")) {
            if (strlen(nameBuffer_) > 0) {
                AddDissolve(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        ImGui::Separator();

        // ディゾルブリスト表示
        ImGui::Text("Dissolves (%d):", static_cast<int>(dissolves_.size()));
        for (int i = 0; i < static_cast<int>(dissolves_.size()); i++) {
            ImGui::PushID(i);

            bool isSelected       = (selectedIndex_ == i);
            std::string labelText = dissolves_[i]->GetGroupName();

            // 再生中のディゾルブを強調表示
            if (dissolves_[i]->IsPlaying()) {
                labelText += " [PLAYING]";
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedIndex_ = i;
            }

            if (dissolves_[i]->IsPlaying()) {
                ImGui::PopStyleColor();
            }

            // 削除ボタン
            ImGui::SameLine();
            if (ImGui::SmallButton("X")) {
                RemoveDissolve(i);
                break;
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択されたディゾルブの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(dissolves_.size())) {
            dissolves_[selectedIndex_]->AdjustParam();
        }

        ImGui::Separator();

        // プレビュー制御
        if (ImGui::CollapsingHeader("Preview Control")) {
            if (preViewObj_) {
                ImGui::Text("Preview Object: Active");

                // 手動でディゾルブthresholdを調整
                static float manualThreshold = 1.0f;
                static bool manualEnable     = false;

                if (ImGui::Button("Apply Manual Values")) {
                    preViewObj_->material_.SetDissolveThreshold(manualThreshold);
                    preViewObj_->material_.SetEnableDissolve(false);
                }

                ImGui::Checkbox("Manual Enable", &manualEnable);
                ImGui::DragFloat("Manual Threshold", &manualThreshold, 0.01f, 0.0f, 1.0f);
            } else {
                ImGui::Text("Preview Object: None");
            }
        }
    }
#endif
}

void DissolveEditor::AddDissolve(const std::string& dissolveName) {
    auto dissolve = std::make_unique<DissolveData>();
    dissolve->Init(dissolveName);
    dissolves_.push_back(std::move(dissolve));
    selectedIndex_ = static_cast<int>(dissolves_.size()) - 1;
}

void DissolveEditor::RemoveDissolve(int index) {
    if (index >= 0 && index < static_cast<int>(dissolves_.size())) {
        dissolves_.erase(dissolves_.begin() + index);

        // 選択インデックス調整
        if (selectedIndex_ >= index) {
            selectedIndex_--;
            if (selectedIndex_ < 0 && !dissolves_.empty()) {
                selectedIndex_ = 0;
            } else if (dissolves_.empty()) {
                selectedIndex_ = -1;
            }
        }
    }
}

void DissolveEditor::AllSaveFile() {
    // すべてのディゾルブデータを保存
    for (auto& dissolve : dissolves_) {
        dissolve->SaveData();
    }
}

DissolveData* DissolveEditor::GetSelectedDissolve() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(dissolves_.size())) {
        return dissolves_[selectedIndex_].get();
    }
    return nullptr;
}

DissolveData* DissolveEditor::GetDissolveByName(const std::string& name) {
    auto it = std::find_if(dissolves_.begin(), dissolves_.end(),
        [&name](const std::unique_ptr<DissolveData>& dissolve) {
            return dissolve->GetGroupName() == name;
        });

    if (it != dissolves_.end()) {
        return it->get();
    }
    return nullptr;
}
