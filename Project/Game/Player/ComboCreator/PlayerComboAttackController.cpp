#include "PlayerComboAttackController.h"
#include "Frame/Frame.h"
// std
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
                attack->SetController(this);
                attacks_.push_back(std::move(attack));
            }
        }
    }
}

void PlayerComboAttackController::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("攻撃作成エディター")) {
        ImGui::PushID("Attack Creator Manager");

        ImGui::Text("Attack Edit:");

        // 新規追加
        ImGui::InputText("新しい攻撃名", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("攻撃を追加")) {
            if (strlen(nameBuffer_) > 0) {
                AddAttack(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        // コンボリスト
        ImGui::Separator();
        VisualizeComboFlow();
        ImGui::Separator();

        // プレビューUI
        DrawPreviewUI();

        // 選択された攻撃データの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
            attacks_[selectedIndex_]->AdjustParam();
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

    // プレビューの更新(常に実行)
    preview_.Update(KetaEngine::Frame::DeltaTime());
#endif
}

void PlayerComboAttackController::DrawPreviewUI() {
    if (!ImGui::CollapsingHeader("プレビュー設定")) {
        return;
    }

    ImGui::PushID("PreviewControl");

    // 再生モード選択
    ImGui::Text("再生モード:");
    ImGui::SameLine();

    auto currentMode = preview_.GetCurrentMode();
    if (ImGui::RadioButton("単体", currentMode == AttackPreviewMode::SINGLE)) {
        preview_.SetPlayMode(AttackPreviewMode::SINGLE);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("連続", currentMode == AttackPreviewMode::CONTINUOUS)) {
        preview_.SetPlayMode(AttackPreviewMode::CONTINUOUS);
    }

    ImGui::Separator();

    // 選択された攻撃があればプレビュー開始ボタンを表示
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(attacks_.size())) {
        PlayerComboAttackData* selectedAttack = attacks_[selectedIndex_].get();

        if (!preview_.IsPlaying()) {
            // プレビュー開始ボタン
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

            if (ImGui::Button("プレビュー開始", ImVec2(-1, 0))) {
                preview_.StartPreview(selectedAttack, currentMode);
            }

            ImGui::PopStyleColor(3);
        }
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "攻撃を選択してください");
    }

    // プレビューUIを描画
    preview_.DrawUI();

    ImGui::PopID();
}

void PlayerComboAttackController::AddAttack(const std::string& attackName) {
    auto attack = std::make_unique<PlayerComboAttackData>();
    attack->Init(attackName);
    attack->SetController(this);
    if (pPlayer_) {
        attack->SetPlayer(pPlayer_);
    }
    attacks_.push_back(std::move(attack));
    selectedIndex_ = static_cast<int>(attacks_.size()) - 1;
}

void PlayerComboAttackController::RemoveAttack(int index) {
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

    // 他から参照されている攻撃を収集
    std::unordered_set<std::string> referencedAttacks;
    for (const auto& attack : attacks_) {
        for (const auto& branch : attack->GetComboBranches()) {
            if (!branch->GetNextAttackName().empty()) {
                referencedAttacks.insert(branch->GetNextAttackName());
            }
        }
    }

    int chainIdx = 0;
    std::unordered_set<std::string> coveredAttacks; // 全ツリー描画で訪れた攻撃を蓄積

    // ルート攻撃（どこからも参照されていない）からチェーンを表示
    for (const auto& attack : attacks_) {
        const std::string& name = attack->GetGroupName();
        if (referencedAttacks.count(name) != 0 || coveredAttacks.count(name) != 0) {
            continue;
        }

        // TreeNodeの開閉に関わらず、処理済みとして登録
        coveredAttacks.insert(name);

        if (chainIdx > 0) {
            ImGui::Separator();
        }

        ImGui::PushID(chainIdx);
        std::string label = "コンボチェイン" + std::to_string(chainIdx + 1);
        if (ImGui::TreeNode(label.c_str())) {
            std::unordered_set<std::string> visited;
            DrawComboFlowTree(name, "", {}, visited);
            coveredAttacks.insert(visited.begin(), visited.end());
            ImGui::TreePop();
        }
        ImGui::PopID();
        ++chainIdx;
    }

    // 孤立した攻撃
    for (const auto& attack : attacks_) {
        const std::string& name = attack->GetGroupName();
        if (coveredAttacks.count(name) != 0 || referencedAttacks.count(name) != 0) {
            continue;
        }

        coveredAttacks.insert(name);

        if (chainIdx > 0) {
            ImGui::Separator();
        }

        ImGui::PushID(chainIdx);
        std::string label = "コンボチェイン" + std::to_string(chainIdx + 1);
        if (ImGui::TreeNode(label.c_str())) {
            std::unordered_set<std::string> visited;
            DrawComboFlowTree(name, "", {}, visited);
            coveredAttacks.insert(visited.begin(), visited.end());
            ImGui::TreePop();
        }
        ImGui::PopID();
        ++chainIdx;
    }

    ImGui::PopID();
#endif
}

void PlayerComboAttackController::DrawComboFlowTree(
    const std::string& attackName,
    const std::string& parentName,
    const std::vector<std::pair<std::string, int>>& siblingBranches,
    std::unordered_set<std::string>& visited) {

    // 循環参照チェック
    if (visited.count(attackName)) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "(→ %s)", attackName.c_str());
        return;
    }

    PlayerComboAttackData* attack = GetAttackByName(attackName);
    if (!attack) {
        return;
    }

    visited.insert(attackName);

    ImGui::PushID(attackName.c_str());

    // 攻撃名の表示
    if (attack->GetAttackParam().isMotionOnly) {
        ImGui::TextColored(ImVec4(1.0f, 0.55f, 0.0f, 1.0f), "%s (Motion Only)", attackName.c_str());
    } else {
        ImGui::TextColored(ImVec4(0.85f, 0.15f, 0.15f, 1.0f), attackName.c_str());
    }

    // クリックで選択
    if (ImGui::IsItemClicked()) {
        SelectAttackByName(attackName);
    }

    // 選択中の強調表示
    if (IsValidIndex(selectedIndex_) && attacks_[selectedIndex_]->GetGroupName() == attackName) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[Selected]");
    }

    // 親の分岐点から渡された代替ブランチボタンを右に表示
    for (const auto& sibling : siblingBranches) {
        ImGui::SameLine();
        ImGui::PushID(sibling.second);
        if (ImGui::SmallButton(("[" + sibling.first + "]").c_str())) {
            activeBranchIndices_[parentName] = sibling.second;
        }
        ImGui::PopID();
    }

    // 分岐を収集
    std::vector<std::string> nextNames;
    for (const auto& branch : attack->GetComboBranches()) {
        const std::string& next = branch->GetNextAttackName();
        if (!next.empty()) {
            nextNames.push_back(next);
        }
    }

    if (nextNames.empty()) {
        ImGui::PopID();
        return;
    }

    // アクティブブランチのインデックスを取得・補正
    int& activeBranchIdx = activeBranchIndices_[attackName];
    if (activeBranchIdx < 0 || activeBranchIdx >= static_cast<int>(nextNames.size())) {
        activeBranchIdx = 0;
    }

    // 次の描画に渡す非アクティブ分岐リストを構築
    std::vector<std::pair<std::string, int>> nextSiblings;
    for (int i = 0; i < static_cast<int>(nextNames.size()); ++i) {
        if (i != activeBranchIdx) {
            nextSiblings.push_back({nextNames[i], i});
        }
    }

    // 矢印
    ImGui::Indent(50.0f);
    ImGui::Text("↓");
    ImGui::Unindent(50.0f);

    ImGui::PopID();

    // アクティブな分岐へ再帰
    DrawComboFlowTree(nextNames[activeBranchIdx], attackName, nextSiblings, visited);
}

bool PlayerComboAttackController::IsFirstAttack(const std::string& attackName) const {
    // 他の攻撃のcomboBranchesから参照されていなければ最初の攻撃
    for (const auto& attack : attacks_) {
        for (const auto& branch : attack->GetComboBranches()) {
            if (branch->GetNextAttackName() == attackName) {
                return false; // 他から参照されている
            }
        }
    }
    return true; // 誰からも参照されていない = 最初の攻撃
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

void PlayerComboAttackController::SetEditorSuite(KetaEngine::EffectEditorSuite* editorSuite) {
    pEditorSuite_ = editorSuite;
}

void PlayerComboAttackController::SetCombo(Combo* combo) {
    pCombo_ = combo;
}

void PlayerComboAttackController::SetPlayer(Player* player) {
    pPlayer_ = player;

    // プレビューにプレイヤーとコントローラーを設定
    preview_.Init(player, this, nullptr);

    for (auto& attack : attacks_) {
        attack->SetPlayer(player);
        attack->SetController(this);
    }
}