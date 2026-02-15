#include "KillCounter.h"
// Player
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
// UI

// imGui
#include <imgui.h>

void KillCounter::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // ランタイム状態リセット
    killCount_    = 0;
    currentLevel_ = 0;

    // ableDefeatLevel == 0 の攻撃だけ解放
    if (pAttackController_) {
        UnlockAttacksForLevel(0);
    }
}

void KillCounter::AddKillCount() {
    killCount_++;
    CheckLevelUp();
}

void KillCounter::CheckLevelUp() {
    // 現在レベルが最大なら何もしない
    if (currentLevel_ >= kMaxKillLevel) {
        return;
    }

    // 閾値に達したらレベルアップ
    if (killCount_ >= levelUpThresholds_[currentLevel_]) {
        currentLevel_++;
        UnlockAttacksForLevel(currentLevel_);
    }
}

void KillCounter::UnlockAttacksForLevel(int32_t level) {
    if (!pAttackController_) {
        return;
    }

    const auto& attacks = pAttackController_->GetAllAttacks();
    for (const auto& attack : attacks) {
        if (!attack) {
            continue;
        }

        auto& param = attack->GetAttackParam();
        if (param.ableDefeatLevel <= level) {
            param.isUnlocked = true;
        }
    }
}

void KillCounter::RegisterParams() {
    for (int32_t i = 0; i < kMaxKillLevel; ++i) {
        globalParameter_->Regist(groupName_, "LevelUpThreshold" + std::to_string(i + 1), &levelUpThresholds_[i]);
    }
}

void KillCounter::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader("KillCounter")) {
        ImGui::PushID("KillCounter");

        ImGui::Text("KillCount: %d", killCount_);
        ImGui::Text("CurrentLevel: %d", currentLevel_);
        ImGui::Separator();

        for (int32_t i = 0; i < kMaxKillLevel; ++i) {
            std::string label = "Level " + std::to_string(i + 1) + " Threshold";
            ImGui::InputInt(label.c_str(), &levelUpThresholds_[i]);
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
