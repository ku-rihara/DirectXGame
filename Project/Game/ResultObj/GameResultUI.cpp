#include "GameResultUI.h"
#include "Frame/Frame.h"
#include "ResultObj/GameResultInfo.h"
#include <imgui.h>

void GameResultUI::Init() {
    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    auto* info   = GameResultInfo::GetInstance();
    currentItem_ = 0;

    items_[0].Init("ResultUI/EnemyKillCountText.dds", "ResultUI/ResultKillUnit.dds",
        info->GetTotalKillCount(), survivalCfg_);
    items_[1].Init("ResultUI/SurvivalTimeCount.dds", "ResultUI/SurvivalTimeCountUnit.dds",
        info->GetSurvivalTimeSec(), killCfg_);
    levelCfg_.digitCount = 1;
    items_[2].Init("ResultUI/MaxLevelCount.dds", "circle.dds",
        info->GetReachedLevel(), levelCfg_);

    items_[0].Start();

    // 常時表示スプライト

    sprite_.reset(KetaEngine::Sprite::Create("ResultUI/ResultPresent.dds"));
    sprite_->SetIsDraw(true);
}

void GameResultUI::Update() {
    float dt = KetaEngine::Frame::DeltaTime();

    // Config の変更を全アイテムに伝播
    items_[0].SetConfig(survivalCfg_);
    items_[1].SetConfig(killCfg_);
    items_[2].SetConfig(levelCfg_);

    // 全アイテム更新
    for (auto& item : items_) {
        item.Update(dt);
    }

    // 現在アイテムが完了したら次へ
    if (currentItem_ < static_cast<int32_t>(items_.size()) && items_[currentItem_].IsFinished()) {
        ++currentItem_;
        if (currentItem_ < static_cast<int32_t>(items_.size())) {
            items_[currentItem_].Start();
        }
    }
}

void GameResultUI::RegisterParams() {
    // ① 生き残った時間
    globalParameter_->Regist(groupName_, "Survival_basePos", &survivalCfg_.basePos);
    globalParameter_->Regist(groupName_, "Survival_labelOffset", &survivalCfg_.labelOffset);
    globalParameter_->Regist(groupName_, "Survival_numberOffset", &survivalCfg_.numberBaseOffset);
    globalParameter_->Regist(groupName_, "Survival_digitSpacing", &survivalCfg_.digitSpacing);
    globalParameter_->Regist(groupName_, "Survival_unitOffset", &survivalCfg_.unitOffset);
    globalParameter_->Regist(groupName_, "Survival_baseScale", &survivalCfg_.baseScale);
    globalParameter_->Regist(groupName_, "Survival_digitScaleOffset", &survivalCfg_.digitScaleOffset);
    globalParameter_->Regist(groupName_, "Survival_startDelay", &survivalCfg_.startDelay);

    // ② 敵を倒した数
    globalParameter_->Regist(groupName_, "Kill_basePos", &killCfg_.basePos);
    globalParameter_->Regist(groupName_, "Kill_labelOffset", &killCfg_.labelOffset);
    globalParameter_->Regist(groupName_, "Kill_numberOffset", &killCfg_.numberBaseOffset);
    globalParameter_->Regist(groupName_, "Kill_digitSpacing", &killCfg_.digitSpacing);
    globalParameter_->Regist(groupName_, "Kill_unitOffset", &killCfg_.unitOffset);
    globalParameter_->Regist(groupName_, "Kill_baseScale", &killCfg_.baseScale);
    globalParameter_->Regist(groupName_, "Kill_digitScaleOffset", &killCfg_.digitScaleOffset);
    globalParameter_->Regist(groupName_, "Kill_startDelay", &killCfg_.startDelay);

    // ③ 到達したレベル
    globalParameter_->Regist(groupName_, "Level_basePos", &levelCfg_.basePos);
    globalParameter_->Regist(groupName_, "Level_labelOffset", &levelCfg_.labelOffset);
    globalParameter_->Regist(groupName_, "Level_numberOffset", &levelCfg_.numberBaseOffset);
    globalParameter_->Regist(groupName_, "Level_digitSpacing", &levelCfg_.digitSpacing);
    globalParameter_->Regist(groupName_, "Level_unitOffset", &levelCfg_.unitOffset);
    globalParameter_->Regist(groupName_, "Level_baseScale", &levelCfg_.baseScale);
    globalParameter_->Regist(groupName_, "Level_digitScaleOffset", &levelCfg_.digitScaleOffset);
    globalParameter_->Regist(groupName_, "Level_startDelay", &levelCfg_.startDelay);
}

void GameResultUI::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        if (ImGui::TreeNode("生き残った時間")) {
            ImGui::DragFloat2("BasePos", &survivalCfg_.basePos.x, 1.f);
            ImGui::DragFloat2("LabelOffset", &survivalCfg_.labelOffset.x, 1.f);
            ImGui::DragFloat2("NumberOffset", &survivalCfg_.numberBaseOffset.x, 1.f);
            ImGui::DragFloat("DigitSpacing", &survivalCfg_.digitSpacing, 1.f);
            ImGui::DragFloat2("UnitOffset", &survivalCfg_.unitOffset.x, 1.f);
            ImGui::DragFloat2("BaseScale", &survivalCfg_.baseScale.x, 0.01f);
            ImGui::DragFloat2("DigitScaleOffset", &survivalCfg_.digitScaleOffset.x, 0.01f);
            ImGui::DragFloat("StartDelay", &survivalCfg_.startDelay, 0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("敵を倒した数")) {
            ImGui::DragFloat2("BasePos", &killCfg_.basePos.x, 1.f);
            ImGui::DragFloat2("LabelOffset", &killCfg_.labelOffset.x, 1.f);
            ImGui::DragFloat2("NumberOffset", &killCfg_.numberBaseOffset.x, 1.f);
            ImGui::DragFloat("DigitSpacing", &killCfg_.digitSpacing, 1.f);
            ImGui::DragFloat2("UnitOffset", &killCfg_.unitOffset.x, 1.f);
            ImGui::DragFloat2("BaseScale", &killCfg_.baseScale.x, 0.01f);
            ImGui::DragFloat2("DigitScaleOffset", &killCfg_.digitScaleOffset.x, 0.01f);
            ImGui::DragFloat("StartDelay", &killCfg_.startDelay, 0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("到達したレベル")) {
            ImGui::DragFloat2("BasePos", &levelCfg_.basePos.x, 1.f);
            ImGui::DragFloat2("LabelOffset", &levelCfg_.labelOffset.x, 1.f);
            ImGui::DragFloat2("NumberOffset", &levelCfg_.numberBaseOffset.x, 1.f);
            ImGui::DragFloat("DigitSpacing", &levelCfg_.digitSpacing, 1.f);
            ImGui::DragFloat2("UnitOffset", &levelCfg_.unitOffset.x, 1.f);
            ImGui::DragFloat2("BaseScale", &levelCfg_.baseScale.x, 0.01f);
            ImGui::DragFloat2("DigitScaleOffset", &levelCfg_.digitScaleOffset.x, 0.01f);
            ImGui::DragFloat("StartDelay", &levelCfg_.startDelay, 0.01f);
            ImGui::TreePop();
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
#endif
}
