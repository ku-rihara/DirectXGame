#include "PlayerComboAttackController.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void PlayerComboAttackController::Init() {
    // 共通パラメータの初期化
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(commonGroupName_);
    BindCommonParams();
    globalParameter_->SyncParamForGroup(commonGroupName_);

    AllLoadFile();
}

void PlayerComboAttackController::AllLoadFile() {

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

void PlayerComboAttackController::Update(float deltaTime) {
    // すべての攻撃データを更新
    for (auto& attack : attacks_) {
        // 必要に応じて更新処理
        attack;
        deltaTime;
    }
}

void PlayerComboAttackController::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Attack Creator Manager")) {
        ImGui::PushID("Attack Creator Manager");

        ImGui::Text("Attack Edit:");

        // 新規追加
        ImGui::InputText("New Attack Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add Attack")) {
            if (strlen(nameBuffer_) > 0) {
                AddAttack(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        // コンボリスト
        ImGui::Separator();
        VisualizeComboFlow();
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

        // 共通パラメータの調整
        AdjustCommonParam();
    }
#endif
}

void PlayerComboAttackController::AddAttack(const std::string& attackName) {
    auto attack = std::make_unique<PlayerComboAttackData>();
    attack->Init(attackName);
    attacks_.push_back(std::move(attack));
    selectedIndex_ = static_cast<int>(attacks_.size()) - 1;
}

void PlayerComboAttackController::RemoveAttack(const int& index) {
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

void PlayerComboAttackController::AllSaveFile() {
    // 共通パラメータを保存
    globalParameter_->SaveFile(commonGroupName_);

    // すべての攻撃データを保存
    for (auto& attack : attacks_) {
        attack->SaveData();
    }
}

void PlayerComboAttackController::VisualizeComboFlow() {
#ifdef _DEBUG
    ImGui::PushID("ComboFlowVisualization");

    if (attacks_.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No attacks available");
        ImGui::PopID();
        return;
    }

    // 全ての攻撃をチェーンに分類
    std::vector<std::vector<std::string>> comboChains = BuildComboChains();

    // 各チェーンを表示
    for (size_t chainIdx = 0; chainIdx < comboChains.size(); ++chainIdx) {
        ImGui::PushID(static_cast<int>(chainIdx));

        const auto& chain       = comboChains[chainIdx];
        std::string headerLabel = "Combo Chain " + std::to_string(chainIdx + 1);

        if (ImGui::TreeNode(headerLabel.c_str())) {
            // チェーン内の各攻撃を表示
            for (size_t i = 0; i < chain.size(); ++i) {
                const std::string& attackName = chain[i];
                PlayerComboAttackData* attack = GetAttackByName(attackName);

                if (!attack) {
                    continue;
                }

                ImGui::PushID(static_cast<int>(i));
                // 攻撃名を表示
                ImGui::Text("%d:",i);
                ImGui::SameLine();
                if (attack->GetAttackParam().isMotionOnly) {
                    ImGui::TextColored(ImVec4(1.0f, 0.55f, 0.0f, 1.0f), "%s (Motion Only)", attackName.c_str());
                } else {
                    ImGui::TextColored(ImVec4(0.85f, 0.15f, 0.15f, 1.0f), attackName.c_str());
                }
              
                // 選択中の攻撃を強調表示
                if (IsValidIndex(selectedIndex_) && attacks_[selectedIndex_]->GetGroupName() == attackName) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[Selected]");
                }

                // クリックで選択
                if (ImGui::IsItemClicked()) {
                    SelectAttackByName(attackName);
                }

                // 次の攻撃への矢印
                if (i < chain.size() - 1) {
                    ImGui::Indent(50.0f);
                    ImGui::Text("↓");
                    ImGui::Unindent(50.0f);
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        // チェーン間の区切り
        if (chainIdx < comboChains.size() - 1) {
            ImGui::Separator();
        }

        ImGui::PopID();
    }

    ImGui::PopID();

#endif
}

std::vector<std::vector<std::string>> PlayerComboAttackController::BuildComboChains() {
    std::vector<std::vector<std::string>> chains;
    std::unordered_set<std::string> visited;

    // 開始攻撃を見つける
    std::unordered_set<std::string> referencedAttacks;
    for (const auto& attack : attacks_) {
        const std::string& nextAttack = attack->GetAttackParam().nextAttackType;
        if (!nextAttack.empty()) {
            referencedAttacks.insert(nextAttack);
        }
    }

    // 各開始攻撃からチェーンを構築
    for (const auto& attack : attacks_) {
        const std::string& attackName = attack->GetGroupName();
        bool isFirstAttack            = attack->GetAttackParam().triggerParam.isFirstAttack;
        bool isNotReferenced          = (referencedAttacks.find(attackName) == referencedAttacks.end());

        // 開始攻撃の条件
        if ((isFirstAttack || isNotReferenced) && visited.find(attackName) == visited.end()) {
            std::vector<std::string> chain;
            BuildChainRecursive(attackName, chain, visited);
            if (!chain.empty()) {
                chains.push_back(chain);
            }
        }
    }

    // 孤立した攻撃
    for (const auto& attack : attacks_) {
        const std::string& attackName = attack->GetGroupName();
        if (visited.find(attackName) == visited.end()) {
            std::vector<std::string> chain;
            BuildChainRecursive(attackName, chain, visited);
            if (!chain.empty()) {
                chains.push_back(chain);
            }
        }
    }

    return chains;
}

void PlayerComboAttackController::BuildChainRecursive(
    const std::string& attackName,
    std::vector<std::string>& chain,
    std::unordered_set<std::string>& visited) {

    // 循環参照チェック
    if (visited.find(attackName) != visited.end()) {
        return;
    }

    PlayerComboAttackData* attack = GetAttackByName(attackName);
    if (!attack) {
        return;
    }

    // 現在の攻撃を追加
    chain.push_back(attackName);
    visited.insert(attackName);

    // 次の攻撃を再帰的に追加
    const std::string& nextAttack = attack->GetAttackParam().nextAttackType;
    if (!nextAttack.empty()) {
        BuildChainRecursive(nextAttack, chain, visited);
    }
}

void PlayerComboAttackController::SelectAttackByName(const std::string& name) {
    for (size_t i = 0; i < attacks_.size(); ++i) {
        if (attacks_[i]->GetGroupName() == name) {
            selectedIndex_ = static_cast<int>(i);
            break;
        }
    }
}

///==========================================================
/// 共通パラメータバインド
///==========================================================
void PlayerComboAttackController::BindCommonParams() {
    for (int32_t i = 0; i < kComboLevel; ++i) {
        globalParameter_->Regist(commonGroupName_, "AttackSpeedRate" + std::to_string(int(i + 1)), &attackValueForLevel_[i].speedRate);
        globalParameter_->Regist(commonGroupName_, "AttackPowerRate" + std::to_string(int(i + 1)), &attackValueForLevel_[i].powerRate);
    }
}

///==========================================================
/// 共通パラメータ調整
///==========================================================
void PlayerComboAttackController::AdjustCommonParam() {
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

bool PlayerComboAttackController::IsValidIndex(int index) const {
    return index >= 0 && index < static_cast<int>(attacks_.size());
}

PlayerComboAttackData* PlayerComboAttackController::GetSelectedAttack() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
        return attacks_[selectedIndex_].get();
    }
    return nullptr;
}

PlayerComboAttackData* PlayerComboAttackController::GetAttackByName(const std::string& name) {
    auto it = std::find_if(attacks_.begin(), attacks_.end(),
        [&name](const std::unique_ptr<PlayerComboAttackData>& attack) {
            return attack->GetGroupName() == name;
        });

    if (it != attacks_.end()) {
        return it->get();
    }
    return nullptr;
}

float PlayerComboAttackController::GetRealAttackSpeed(float baseTimeSpeed) const {

    float result = baseTimeSpeed * attackValueForLevel_[pCombo_->GetCurrentLevel()].speedRate;

    return result;
}

float PlayerComboAttackController::GetPowerRate() const {

    float rate = attackValueForLevel_[pCombo_->GetCurrentLevel()].powerRate;
    return rate;
}

void PlayerComboAttackController::SetCombo(Combo* combo) {
    pCombo_ = combo;
}
