#include "PlayerComboAttackCreator.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void PlayerComboAttackCreator::Init() {
    // 共通パラメータの初期化
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(commonGroupName_, false);
    BindCommonParams();
    globalParameter_->SyncParamForGroup(commonGroupName_);

    AllLoadFile();
   
}

void PlayerComboAttackCreator::AllLoadFile() {
    // AttackCreatorのPlayerComboAttackDataフォルダ内のすべてのファイルを検索
  
    if (std::filesystem::exists(AttackDataFolderPath_) && std::filesystem::is_directory(AttackDataFolderPath_)) {
        // 既存の攻撃データをクリア
        attacks_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(AttackDataFolderPath_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 共通パラメータファイルはスキップ
                if (fileName == commonGroupName_) {
                    continue;
                }

                // 新規作成してロード
                auto attack = std::make_unique<PlayerComboAttackData>();
                attack->Init(fileName);
                attack->LoadData(); // Load
                attacks_.push_back(std::move(attack));
            }
        }
    }
}

void PlayerComboAttackCreator::Update(const float& deltaTime) {
    // すべての攻撃データを更新
    for (auto& attack : attacks_) {
        // 必要に応じて更新処理
        attack;
        deltaTime;
    }
}

void PlayerComboAttackCreator::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Attack Creator Manager")) {
        ImGui::PushID("Attack Creator Manager");

        // 共通パラメータの調整
        AdjustCommonParam();

        ImGui::Separator();

        ImGui::Text("Preview Object:");
        ImGui::DragFloat3("preViewFollowObj", &basePos_.x, 0.1f);
        ImGui::Checkbox("IsDraw", &isPreViewDraw_);

        ImGui::Separator();
        ImGui::Text("Attack Edit:");

        // 新規追加
        ImGui::InputText("New Attack Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add Attack")) {
            if (strlen(nameBuffer_) > 0) {
                AddAttack(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        ImGui::Separator();

        // 攻撃リスト表示
        ImGui::Text("Attacks (%d):", static_cast<int>(attacks_.size()));
        for (int i = 0; i < static_cast<int>(attacks_.size()); i++) {
            ImGui::PushID(i);

            bool isSelected       = (selectedIndex_ == i);
            std::string labelText = attacks_[i]->GetGroupName();

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedIndex_ = i;
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択された攻撃データの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
            attacks_[selectedIndex_]->AdjustParam();
        }

        ImGui::Separator();
        ImGui::Text("File Operations:");

        // 選択中の攻撃データ個別のセーブ/ロード
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
            ImGui::SeparatorText("Selected Attack File Operations");

            auto* selectedAttack   = attacks_[selectedIndex_].get();
            std::string attackName = selectedAttack->GetGroupName();

            // 個別Load
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
            if (ImGui::Button(("Load " + attackName).c_str())) {
                selectedAttack->LoadData();
                MessageBoxA(nullptr, (attackName + " loaded successfully.").c_str(), "Attack Creator", 0);
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();

            // 個別Save
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
            if (ImGui::Button(("Save " + attackName).c_str())) {
                selectedAttack->SaveData();
                MessageBoxA(nullptr, (attackName + " saved successfully.").c_str(), "Attack Creator", 0);
            }
            ImGui::PopStyleColor(3);
        }
        ImGui::SeparatorText("All File Save/Load");

        // Load ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("Load All Attacks")) {
            AllLoadFile();
            MessageBoxA(nullptr, "All Attacks loaded successfully.", "Attack Creator", 0);
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        // Save ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
        if (ImGui::Button("Save All Attacks")) {
            AllSaveFile();
            MessageBoxA(nullptr, "All Attacks saved successfully.", "Attack Creator", 0);
        }
        ImGui::PopStyleColor(3);

        ImGui::PopID();
    }
#endif
}

void PlayerComboAttackCreator::AddAttack(const std::string& attackName) {
    auto attack = std::make_unique<PlayerComboAttackData>();
    attack->Init(attackName);
    attacks_.push_back(std::move(attack));
    selectedIndex_ = static_cast<int>(attacks_.size()) - 1;
}

void PlayerComboAttackCreator::RemoveAttack(const int& index) {
    if (index >= 0 && index < static_cast<int>(attacks_.size())) {
        attacks_.erase(attacks_.begin() + index);

        // 選択インデックス調整
        if (selectedIndex_ >= index) {
            selectedIndex_--;
            if (selectedIndex_ < 0 && !attacks_.empty()) {
                selectedIndex_ = 0;
            } else if (attacks_.empty()) {
                selectedIndex_ = -1;
            }
        }
    }
}

void PlayerComboAttackCreator::AllSaveFile() {
    // 共通パラメータを保存
    globalParameter_->SaveFile(commonGroupName_);

    // すべての攻撃データを保存
    for (auto& attack : attacks_) {
        attack->SaveData();
    }
}

PlayerComboAttackData* PlayerComboAttackCreator::GetSelectedAttack() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
        return attacks_[selectedIndex_].get();
    }
    return nullptr;
}

PlayerComboAttackData* PlayerComboAttackCreator::GetAttackByName(const std::string& name) {
    auto it = std::find_if(attacks_.begin(), attacks_.end(),
        [&name](const std::unique_ptr<PlayerComboAttackData>& attack) {
            return attack->GetGroupName() == name;
        });

    if (it != attacks_.end()) {
        return it->get();
    }
    return nullptr;
}

///==========================================================
/// 共通パラメータバインド
///==========================================================
void PlayerComboAttackCreator::BindCommonParams() {
    for (int32_t i = 0; i < kComboLevel; ++i) {
        globalParameter_->Bind(commonGroupName_, "AttackSpeedRate" + std::to_string(int(i + 1)), &attackValueForLevel_[i].speedRate);
        globalParameter_->Bind(commonGroupName_, "AttackPowerRate" + std::to_string(int(i + 1)), &attackValueForLevel_[i].powerRate);
    }
}

///==========================================================
/// 共通パラメータ調整
///==========================================================
void PlayerComboAttackCreator::AdjustCommonParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Common Combo Parameters")) {
        ImGui::PushID("CommonComboParams");

        // attackLevel Values
        for (int32_t i = 0; i < kComboLevel; ++i) {
            ImGui::SeparatorText(("AttackValue Level " + std::to_string(int(i + 1))).c_str());
            ImGui::DragFloat(("AttackSpeedRate" + std::to_string(int(i + 1))).c_str(), &attackValueForLevel_[i].speedRate, 0.01f);
            ImGui::DragFloat(("AttackPowerRate" + std::to_string(int(i + 1))).c_str(), &attackValueForLevel_[i].powerRate, 0.01f);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(commonGroupName_);
        globalParameter_->ParamLoadForImGui(commonGroupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}