#include "EnemyDamageReactionData.h"
#include "Function/GetFile.h"
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void EnemyDamageReactionData::Init(const std::string& reactionName) {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();
    // グループ名設定
    groupName_ = reactionName;

    // 新規登録
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    reactionParam_.reactionState = static_cast<ReactionState>(reactionParam_.intReactionState);
}

void EnemyDamageReactionData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    // 演出データをロード
    LoadRenditions();

    globalParameter_->SyncParamForGroup(groupName_);
}

void EnemyDamageReactionData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);

    // すべての演出データを保存
    SaveAllRenditions();
}

///==========================================================
/// 演出データ管理
///==========================================================
void EnemyDamageReactionData::AddRendition() {
    int32_t newIndex  = static_cast<int32_t>(renditions_.size());
    auto newRendition = std::make_unique<EnemyDamageRenditionData>();
    newRendition->Init(groupName_, newIndex);

    renditions_.push_back(std::move(newRendition));
    selectedRenditionIndex_ = newIndex;
}

void EnemyDamageReactionData::RemoveRendition(const int32_t& index) {
    if (index >= 0 && index < static_cast<int32_t>(renditions_.size())) {
        renditions_.erase(renditions_.begin() + index);

        // 選択インデックスの調整
        if (selectedRenditionIndex_ >= index) {
            selectedRenditionIndex_--;
            if (selectedRenditionIndex_ < 0 && !renditions_.empty()) {
                selectedRenditionIndex_ = 0;
            }
        }

        // インデックスの再設定
        for (int32_t i = 0; i < static_cast<int32_t>(renditions_.size()); ++i) {
            renditions_[i]->Init(groupName_, i);
        }
    }
}

void EnemyDamageReactionData::ClearRenditions() {
    renditions_.clear();
    selectedRenditionIndex_ = -1;
}

void EnemyDamageReactionData::InitRenditions() {
    for (int32_t i = 0; i < static_cast<int32_t>(renditions_.size()); ++i) {
        renditions_[i]->Init(groupName_, i);
    }
}

void EnemyDamageReactionData::SaveAllRenditions() {
    for (auto& rendition : renditions_) {
        rendition->SaveData();
    }
}

void EnemyDamageReactionData::LoadRenditions() {
    // reactionName専用のフォルダパスを作成
    std::string folderPath      = renditionFolderPath_ + groupName_ + "/";
    std::string renditionPrefix = groupName_ + "_Rendition";

    if (std::filesystem::exists(folderPath) && std::filesystem::is_directory(folderPath)) {
        std::vector<std::pair<int32_t, std::string>> renditionFiles;

        // 演出ファイルを検索
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string fileName = entry.path().stem().string();

                // ファイル名チェック
                if (fileName.find(renditionPrefix) == 0) {
                    // インデックス番号を抽出
                    std::string indexStr = fileName.substr(renditionPrefix.length());
                    int32_t index        = std::stoi(indexStr);
                    renditionFiles.emplace_back(index, fileName);
                }
            }
        }

        // インデックス順にソート
        std::sort(renditionFiles.begin(), renditionFiles.end());

        CreateOrLoadRendition(renditionFiles);
    }
}

void EnemyDamageReactionData::CreateOrLoadRendition(
    const std::vector<std::pair<int32_t, std::string>>& renditionFiles) {

    if (renditions_.size() == 0) {
        // 既存の演出をクリア
        ClearRenditions();
        // 演出を作成してロード
        for (const auto& [index, fileName] : renditionFiles) {
            auto newRendition = std::make_unique<EnemyDamageRenditionData>();
            newRendition->Init(groupName_, index);
            newRendition->LoadData();
            renditions_.push_back(std::move(newRendition));
        }
    } else {
        // すべての演出をロード
        for (auto& rendition : renditions_) {
            rendition->LoadData();
        }
    }
}

///==========================================================
/// バインド
///==========================================================
void EnemyDamageReactionData::RegisterParams() {
    globalParameter_->Regist(groupName_, "TriggerAttackName", &reactionParam_.triggerAttackName);
    globalParameter_->Regist(groupName_, "ReactionState", &reactionParam_.intReactionState);
    globalParameter_->Regist(groupName_, "damageCollingTime", &reactionParam_.damageCollingTime);

    // SlammedParam
    globalParameter_->Regist(groupName_, "Slammed_BoundNum", &reactionParam_.slammedParam.boundNum);
    globalParameter_->Regist(groupName_, "Slammed_BounceDamping", &reactionParam_.slammedParam.bounceDamping);

    // TakeUpperParam
    globalParameter_->Regist(groupName_, "TakeUpper_FloatingTime", &reactionParam_.takeUpperParam.floatingTime);
}

///==========================================================
/// パラメータ調整
///==========================================================
void EnemyDamageReactionData::AdjustParam() {
#ifdef _DEBUG

    ImGui::PushID(groupName_.c_str());

    // Trigger Attack
    ImGui::SeparatorText("Trigger Attack");
    SelectTriggerAttack();

    ImGui::Separator();

    // Reaction State
    ImGui::SeparatorText("Simple Param");
    ImGui::DragFloat("damageCollingTime", &reactionParam_.damageCollingTime, 0.01f);


    ImGui::SeparatorText("Reaction State");
    const char* stateNames[] = {"Normal", "Slammed", "TakeUpper"};
    ImGui::Combo("Reaction Type", &reactionParam_.intReactionState, stateNames, IM_ARRAYSIZE(stateNames));

    // 状態に応じたパラメータ表示
    reactionParam_.reactionState = static_cast<ReactionState>(reactionParam_.intReactionState);

    if (reactionParam_.reactionState == ReactionState::Slammed) {
        ImGui::SeparatorText("Slammed Parameters");
        ImGui::DragInt("Bound Number", &reactionParam_.slammedParam.boundNum, 1.0f, 0, 10);
        ImGui::DragFloat("Bounce Damping", &reactionParam_.slammedParam.bounceDamping, 0.01f, 0.0f, 1.0f);
    } else if (reactionParam_.reactionState == ReactionState::TakeUpper) {
        ImGui::SeparatorText("Take Upper Parameters");
        ImGui::DragFloat("Floating Time", &reactionParam_.takeUpperParam.floatingTime, 0.01f, 0.0f, 10.0f);
    }

    ImGui::Separator();

    // 演出リスト
    if (showRenditionList_) {
        ImGui::SeparatorText("Renditions");
        ImGui::Text("Renditions (%zu):", renditions_.size());

        for (int32_t i = 0; i < static_cast<int32_t>(renditions_.size()); ++i) {
            ImGui::PushID(i);

            bool isSelected       = (selectedRenditionIndex_ == i);
            std::string labelText = "Rendition " + std::to_string(i);

            if (ImGui::Selectable(labelText.c_str(), isSelected, 0, ImVec2(0, 0))) {
                selectedRenditionIndex_ = i;
            }

            ImGui::PopID();
            ImGui::Spacing();
        }

        if (ImGui::Button("Add Rendition")) {
            AddRendition();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear All Renditions")) {
            ClearRenditions();
        }
    }

    ImGui::Separator();

    // 選択された演出の調整
    if (selectedRenditionIndex_ >= 0 && selectedRenditionIndex_ < static_cast<int32_t>(renditions_.size())) {
        renditions_[selectedRenditionIndex_]->AdjustParam();
    }

    ImGui::PopID();

#endif // _DEBUG
}

void EnemyDamageReactionData::SelectTriggerAttack() {
    fileSelector_.SelectFile(
        "Trigger Attack Type",
        "Resources/GlobalParameter/AttackCreator",
        reactionParam_.triggerAttackName,
        groupName_,
        true);
}

const EnemyDamageRenditionData* EnemyDamageReactionData::GetSelectedRendition() const {
    if (selectedRenditionIndex_ >= 0 && selectedRenditionIndex_ < static_cast<int32_t>(renditions_.size())) {
        return renditions_[selectedRenditionIndex_].get();
    }
    return nullptr;
}

void EnemyDamageReactionData::SetSelectedRenditionIndex(const int32_t& index) {
    if (index >= -1 && index < static_cast<int32_t>(renditions_.size())) {
        selectedRenditionIndex_ = index;
    }
}