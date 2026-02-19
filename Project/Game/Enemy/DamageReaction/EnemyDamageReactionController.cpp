#include "EnemyDamageReactionController.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <Windows.h>

void EnemyDamageReactionController::Init() {
    // デフォルトパラメータの初期化（敵タイプ別）
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(defaultParamGroupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(defaultParamGroupName_);

    AllLoadFile();
}

void EnemyDamageReactionController::RegisterParams() {
    // 敵タイプの名前
    const char* typeNames[]     = {"Normal", "Strong"};
    // アニメーション種類の名前
    const char* animeTypeNames[] = {"Normal", "TakeUpper", "Slammed", "Bound", "GetUp"};
    for (int i = 0; i < kEnemyTypeCount; ++i) {
        std::string typePrefix = std::string(typeNames[i]) + "_";
        // アニメーション種類別に登録
        for (int a = 0; a < static_cast<int>(DefaultAnimType::Count); ++a) {
            globalParameter_->Regist(defaultParamGroupName_,
                typePrefix + animeTypeNames[a] + "_DefaultDamageAnimationName",
                &defaultDamageAnimationNames_[i][a]);
        }
        // イージング系は敵タイプ別のみ
        globalParameter_->Regist(defaultParamGroupName_, typePrefix + "DefaultObjEaseAnimationName", &defaultObjEaseAnimationNames_[i]);
        globalParameter_->Regist(defaultParamGroupName_, typePrefix + "DefaultObjEaseAnimationStartTiming", &defaultObjEaseAnimationStartTimings_[i]);
    }
}

void EnemyDamageReactionController::AllLoadFile() {

    // AttackCreatorのPlayerComboAttackDataフォルダ内のすべてのファイルを検索
    if (std::filesystem::exists(AttackDataFolderPath_) && std::filesystem::is_directory(AttackDataFolderPath_)) {
        // 既存の攻撃データをクリア
        reactions_.clear();
        selectedIndex_ = -1;

        for (const auto& entry : std::filesystem::directory_iterator(AttackDataFolderPath_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // 新規作成してロード
                auto attack = std::make_unique<EnemyDamageReactionData>();
                attack->Init(fileName);
                attack->LoadData(); // Load
                reactions_.push_back(std::move(attack));
            }
        }
    }
}

void EnemyDamageReactionController::Update(float deltaTime) {
    // すべての攻撃データを更新
    for (auto& attack : reactions_) {
        // 必要に応じて更新処理
        attack;
        deltaTime;
    }
}

void EnemyDamageReactionController::EditorUpdate() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("Damage Reaction Manager")) {
        ImGui::PushID("Damage Reaction Manager");

        // デフォルトパラメータUI（敵タイプ別）
        if (ImGui::TreeNode("Default Parameters")) {
            const char* typeNames[] = {"Normal", "Strong"};
            const auto& availableAnimes = availableAnimations_;

            const char* animeTypeLabels[] = {"Normal Damage", "TakeUpper", "Slammed", "Bound", "GetUp"};

            for (int t = 0; t < kEnemyTypeCount; ++t) {
                ImGui::PushID(t);
                if (ImGui::TreeNode(typeNames[t])) {
                    // アニメーション種類別のドロップダウン
                    for (int a = 0; a < static_cast<int>(DefaultAnimType::Count); ++a) {
                        ImGui::PushID(a);
                        if (!availableAnimes.empty()) {
                            int currentIndex = 0;
                            for (size_t i = 0; i < availableAnimes.size(); ++i) {
                                if (availableAnimes[i] == defaultDamageAnimationNames_[t][a]) {
                                    currentIndex = static_cast<int>(i + 1);
                                    break;
                                }
                            }

                            std::vector<const char*> items;
                            items.push_back("None");
                            for (const auto& anime : availableAnimes) {
                                items.push_back(anime.c_str());
                            }

                            if (ImGui::Combo(animeTypeLabels[a], &currentIndex, items.data(), static_cast<int>(items.size()))) {
                                if (currentIndex == 0) {
                                    defaultDamageAnimationNames_[t][a].clear();
                                } else {
                                    defaultDamageAnimationNames_[t][a] = availableAnimes[currentIndex - 1];
                                }
                            }
                        }
                        ImGui::PopID();
                    }

                    ImGui::Separator();

                    // イージングアニメーション選択
                    defaultObjEaseFileSelectors_[t].SelectFile(
                        "Obj Ease Animation",
                        "Resources/GlobalParameter/ObjEaseAnimation/Enemy/Dates/",
                        defaultObjEaseAnimationNames_[t],
                        "",
                        true);
                    ImGui::DragFloat("Ease Start Timing", &defaultObjEaseAnimationStartTimings_[t], 0.01f);

                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            // デフォルトパラメータ保存
            if (ImGui::Button("Save Default Parameters")) {
                globalParameter_->SaveFile(defaultParamGroupName_, defaultParamFolderPath_);
            }
            ImGui::TreePop();
        }

        ImGui::Separator();
        ImGui::Text("Damage Reaction Edit:");

        // 新規追加
        ImGui::InputText("New Reaction Name", nameBuffer_, IM_ARRAYSIZE(nameBuffer_));
        if (ImGui::Button("Add Reaction")) {
            if (strlen(nameBuffer_) > 0) {
                AddAttack(nameBuffer_);
                nameBuffer_[0] = '\0';
            }
        }

        ImGui::Separator();

        // 攻撃リスト表示
        ImGui::Text("Reactions (%d):", static_cast<int>(reactions_.size()));
        for (int i = 0; i < static_cast<int>(reactions_.size()); i++) {
            ImGui::PushID(i);

            bool isSelected       = (selectedIndex_ == i);
            std::string labelText = reactions_[i]->GetGroupName();

            if (ImGui::Selectable(labelText.c_str(), isSelected)) {
                selectedIndex_ = i;
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // 選択された攻撃データの編集
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(reactions_.size())) {
            reactions_[selectedIndex_]->AdjustParam(availableAnimations_);
        }

        ImGui::Separator();
        ImGui::Text("File Operations:");

        // 選択中の攻撃データ個別のセーブ/ロード
        if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(reactions_.size())) {
            ImGui::SeparatorText("Selected Attack File Operations");

            auto* selectedAttack   = reactions_[selectedIndex_].get();
            std::string attackName = selectedAttack->GetGroupName();

            // 個別Load
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
            if (ImGui::Button(("Load " + attackName).c_str())) {
                selectedAttack->LoadData();
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

       
        ImGui::PopID();

      
    }
#endif
}

void EnemyDamageReactionController::AddAttack(const std::string& attackName) {
    auto attack = std::make_unique<EnemyDamageReactionData>();
    attack->Init(attackName);
    reactions_.push_back(std::move(attack));
    selectedIndex_ = static_cast<int>(reactions_.size()) - 1;
}

void EnemyDamageReactionController::RemoveAttack(int index) {
    if (index >= 0 && index < static_cast<int>(reactions_.size())) {
        reactions_.erase(reactions_.begin() + index);

        // 選択インデックス調整
        if (selectedIndex_ >= index) {
            selectedIndex_--;
            if (selectedIndex_ < 0 && !reactions_.empty()) {
                selectedIndex_ = 0;
            } else if (reactions_.empty()) {
                selectedIndex_ = -1;
            }
        }
    }
}

void EnemyDamageReactionController::AllSaveFile() {
    // すべての攻撃データを保存
    for (auto& attack : reactions_) {
        attack->SaveData();
    }
}

EnemyDamageReactionData* EnemyDamageReactionController::GetSelectedAttack() {
    if (selectedIndex_ >= 0 && selectedIndex_ < static_cast<int>(reactions_.size())) {
        return reactions_[selectedIndex_].get();
    }
    return nullptr;
}

EnemyDamageReactionData* EnemyDamageReactionController::GetAttackByTriggerName(const std::string& name) {
    auto it = std::find_if(reactions_.begin(), reactions_.end(),
        [&name](const std::unique_ptr<EnemyDamageReactionData>& attack) {
            return attack->GetReactionParam().triggerAttackName == name;
        });

    if (it != reactions_.end()) {
        return it->get();
    }
    return nullptr;
}

