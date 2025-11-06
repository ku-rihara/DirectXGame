//#include "PlayerEasingAnimation.h"
//#include <algorithm>
//#include <filesystem>
//#include <imgui.h>
//#include <Windows.h>
//
//void PlayerEasingAnimation::Init() {
//  
//    AllLoadFile();
//}
//
//void PlayerEasingAnimation::AllLoadFile() {
//
//    // AttackCreatorのPlayerEasingAnimationDataフォルダ内のすべてのファイルを検索
//    if (std::filesystem::exists(AttackDataFolderPath_) && std::filesystem::is_directory(AttackDataFolderPath_)) {
//        // 既存の攻撃データをクリア
//        attacks_.clear();
//        selectedIndex_ = -1;
//
//        for (const auto& entry : std::filesystem::directory_iterator(AttackDataFolderPath_)) {
//            if (entry.is_regular_file() && entry.path().extension() == ".json") {
//                std::string fileName = entry.path().stem().string();
//
//
//                // 新規作成してロード
//                auto attack = std::make_unique<PlayerEasingAnimationData>();
//                attack->Init(fileName);
//                attack->LoadData(); // Load
//                attacks_.push_back(std::move(attack));
//            }
//        }
//    }
//}
//
//void PlayerEasingAnimation::Update(const float& deltaTime) {
//    // すべての攻撃データを更新
//    for (auto& attack : attacks_) {
//        // 必要に応じて更新処理
//        attack;
//        deltaTime;
//    }
//}
//
//void PlayerEasingAnimation::EditorUpdate() {
//#ifdef _DEBUG
//    if (ImGui::CollapsingHeader("Attack Creator Manager")) {
//        ImGui::PushID("Attack Creator Manager");
//
//        ImGui::Text("Attack Edit:");
//
//        // 新規追加
//        ImGui::InputText("New Attack Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
//        if (ImGui::Button("Add Attack")) {
//            if (strlen(nameBuffer_) > 0) {
//                AddAttack(nameBuffer_);
//                nameBuffer_[0] = '\0';
//            }
//        }
//
//        ImGui::Separator();
//
//        // 攻撃リスト表示
//        ImGui::Text("Attacks (%d):", static_cast<int>(attacks_.size()));
//        for (int i = 0; i < static_cast<int>(attacks_.size()); i++) {
//            ImGui::PushID(i);
//
//            bool isSelected       = (selectedIndex_ == i);
//            std::string labelText = attacks_[i]->GetGroupName();
//
//            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
//                selectedIndex_ = i;
//            }
//
//            ImGui::PopID();
//        }
//
//        ImGui::Separator();
//
//        // 選択された攻撃データの編集
//        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
//            attacks_[selectedIndex_]->AdjustParam();
//        }
//
//        ImGui::Separator();
//        ImGui::Text("File Operations:");
//
//        // 選択中の攻撃データ個別のセーブ/ロード
//        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
//            ImGui::SeparatorText("Selected Attack File Operations");
//
//            auto* selectedAttack   = attacks_[selectedIndex_].get();
//            std::string attackName = selectedAttack->GetGroupName();
//
//            // 個別Load
//            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
//            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
//            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
//            if (ImGui::Button(("Load " + attackName).c_str())) {
//                selectedAttack->LoadData();
//                MessageBoxA(nullptr, (attackName + " loaded successfully.").c_str(), "Attack Creator", 0);
//            }
//            ImGui::PopStyleColor(3);
//            ImGui::SameLine();
//
//            // 個別Save
//            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
//            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
//            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
//            if (ImGui::Button(("Save " + attackName).c_str())) {
//                selectedAttack->SaveData();
//                MessageBoxA(nullptr, (attackName + " saved successfully.").c_str(), "Attack Creator", 0);
//            }
//            ImGui::PopStyleColor(3);
//        }
//        ImGui::SeparatorText("All File Save/Load");
//
//        // Load ボタン
//        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
//        if (ImGui::Button("Load All Attacks")) {
//            AllLoadFile();
//            MessageBoxA(nullptr, "All Attacks loaded successfully.", "Attack Creator", 0);
//        }
//        ImGui::PopStyleColor(3);
//        ImGui::SameLine();
//
//        // Save ボタン
//        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
//        if (ImGui::Button("Save All Attacks")) {
//            AllSaveFile();
//            MessageBoxA(nullptr, "All Attacks saved successfully.", "Attack Creator", 0);
//        }
//        ImGui::PopStyleColor(3);
//        ImGui::PopID();
//
//        // 共通パラメータの調整
//        AdjustCommonParam();
//    }
//#endif
//}
//
//void PlayerEasingAnimation::AddAttack(const std::string& attackName) {
//    auto attack = std::make_unique<PlayerEasingAnimationData>();
//    attack->Init(attackName);
//    attacks_.push_back(std::move(attack));
//    selectedIndex_ = static_cast<int>(attacks_.size()) - 1;
//}
//
//void PlayerEasingAnimation::RemoveAttack(const int& index) {
//    if (index >= 0 && index < static_cast<int>(attacks_.size())) {
//        attacks_.erase(attacks_.begin() + index);
//
//        // 選択インデックス調整
//        if (selectedIndex_ >= index) {
//            selectedIndex_--;
//            if (selectedIndex_ < 0 && !attacks_.empty()) {
//                selectedIndex_ = 0;
//            } else if (attacks_.empty()) {
//                selectedIndex_ = -1;
//            }
//        }
//    }
//}
//
//void PlayerEasingAnimation::AllSaveFile() {
//  
//    // すべての攻撃データを保存
//    for (auto& attack : attacks_) {
//        attack->SaveData();
//    }
//}
//
//PlayerEasingAnimationData* PlayerEasingAnimation::GetSelectedAttack() {
//    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
//        return attacks_[selectedIndex_].get();
//    }
//    return nullptr;
//}
//
//PlayerEasingAnimationData* PlayerEasingAnimation::GetAttackByName(const std::string& name) {
//    auto it = std::find_if(attacks_.begin(), attacks_.end(),
//        [&name](const std::unique_ptr<PlayerEasingAnimationData>& attack) {
//            return attack->GetGroupName() == name;
//        });
//
//    if (it != attacks_.end()) {
//        return it->get();
//    }
//    return nullptr;
//}
