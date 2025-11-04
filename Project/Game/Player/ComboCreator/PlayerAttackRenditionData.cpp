#include "PlayerAttackRenditionData.h"
#include "Function/GetFile.h"
#include "input/InputData.h"
#include <algorithm>
#include <imgui.h>

void PlayerAttackRenditionData::Init(const std::string& attackName) {
    // グローバルパラメータ
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = attackName;
    globalParameter_->CreateGroup(groupName_, true);

    // バインド
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // condition適応
    attackParam_.triggerParam.condition = static_cast<TriggerCondition>(tempCondition_);
}

void PlayerAttackRenditionData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void PlayerAttackRenditionData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

///==========================================================
/// バインド
///==========================================================
void PlayerAttackRenditionData::BindParams() {
    // simpleParam
    globalParameter_->Bind(groupName_, "power", &attackParam_.power);
    globalParameter_->Bind(groupName_, "KnockBackPower", &attackParam_.knockBackPower);

   
}

///==========================================================
/// パラメータ調整
///==========================================================
void PlayerAttackRenditionData::AdjustParam() {
#ifdef _DEBUG

    ImGui::PushID(groupName_.c_str());

    // Simple Parameter
    ImGui::SeparatorText("simple Parameter");
    ImGui::DragFloat("Power", &attackParam_.power, 0.01f);
    ImGui::DragFloat("KnockBack Power", &attackParam_.knockBackPower, 0.01f);

    // next Attack
    ImGui::SeparatorText("Next Attack");
    SelectNextAttack();

    // セーブ・ロード
    ImGui::Separator();
    globalParameter_->ParamSaveForImGui(groupName_, folderPath_);
    globalParameter_->ParamLoadForImGui(groupName_, folderPath_);

    ImGui::PopID();

#endif // _DEBUG
}

void PlayerAttackRenditionData::SelectNextAttack() {
    if (needsRefresh_ || attackFileNames_.empty()) {
        attackFileNames_ = GetFileNamesForDyrectry("Resources/GlobalParameter/AttackCreator");

        // 自身のファイルを候補から削除
        attackFileNames_.erase(
            std::remove_if(attackFileNames_.begin(), attackFileNames_.end(),
                [this](const std::string& name) {
                    return name == groupName_;
                }),
            attackFileNames_.end());

        // 先頭に"None"を追加
        attackFileNames_.insert(attackFileNames_.begin(), "None");

        needsRefresh_ = false;
    }

    // 現在選択中のインデックスを取得
    int currentIndex = 0;
    for (int i = 0; i < attackFileNames_.size(); i++) {
        if (attackFileNames_[i] == attackParam_.nextAttackType) {
            currentIndex = i;
            break;
        }
    }

    // comboItemsに順番に追加していく
    std::vector<const char*> comboItems;
    for (const auto& name : attackFileNames_) {
        comboItems.push_back(name.c_str());
    }

    // コンボボックスで選択
    if (ImGui::Combo("Next Attack Type", &currentIndex, comboItems.data(), static_cast<int>(comboItems.size()))) {
        attackParam_.nextAttackType = attackFileNames_[currentIndex];
    }

    // リフレッシュボタン
    ImGui::SameLine();
    if (ImGui::SmallButton("Refresh")) {
        needsRefresh_ = true;
    }
}
