#include "ComboBranchParameter.h"
#include "input/InputData.h"
#include <imgui.h>

void ComboBranchParameter::Init(const std::string& groupName, int32_t branchIndex) {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    branchIndex_     = branchIndex;

    // グループ作成、パラメータ登録、同期
    groupName_ = groupName;
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void ComboBranchParameter::RegisterParams() {
    std::string prefix = "Branch_" + std::to_string(branchIndex_) + "_";

    globalParameter_->Regist(groupName_, prefix + "nextAttackName", &nextAttackName_);
    globalParameter_->Regist(groupName_, prefix + "keyboardButton", &keyboardButton_);
    globalParameter_->Regist(groupName_, prefix + "gamepadButton", &gamepadButton_);
    globalParameter_->Regist(groupName_, prefix + "cancelTime", &cancelTime_);
    globalParameter_->Regist(groupName_, prefix + "precedeInputTime", &precedeInputTime_);
    globalParameter_->Regist(groupName_, prefix + "requireHit", &requireHit_);
}

bool ComboBranchParameter::DrawParameterUI(const std::string& attackFolderPath, const std::string& parentGroupName) {
    attackFolderPath;
    parentGroupName;
#ifdef _DEBUG
    bool requestDelete = false;

    // 分岐ヘッダー
    std::string header = "分岐 " + std::to_string(branchIndex_ + 1);
    if (!nextAttackName_.empty()) {
        header += " -> " + nextAttackName_;
    }

    if (ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();

        // 次の攻撃名選択
        fileSelector_.SelectFile(
            "次の攻撃名",
            attackFolderPath,
            nextAttackName_,
            parentGroupName + "_branch_" + std::to_string(branchIndex_),
            true);

        // ボタン設定
        ImGui::Separator();
        ImGui::Text("入力ボタン:");
        ImGuiKeyboardKeySelector("キーボード", keyboardButton_);
        ImGuiGamepadButtonSelector("ゲームパッド", gamepadButton_);

        // タイミング設定
        ImGui::Separator();
        ImGui::Text("タイミング設定:");
        ImGui::DragFloat("キャンセル可能時間", &cancelTime_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("先行入力受付時間", &precedeInputTime_, 0.01f, 0.0f, 10.0f);

        // 条件設定
        ImGui::Separator();
        ImGui::Text("発動条件:");
        ImGui::Checkbox("ヒット必須", &requireHit_);

        // 削除ボタン
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("この分岐を削除")) {
            requestDelete = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::Unindent();
    }

    return requestDelete;
#else
    return false;
#endif
}