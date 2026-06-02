#include "GameResultUI.h"
#include "Frame/Frame.h"
#include "ResultObj/GameResultInfo.h"
#include <imgui.h>

void GameResultUI::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();

    // ResultUI グループ（位置・スケール）
    globalParameter_->CreateGroup(groupName_);
    // RankThreshold グループ（ランク判定スコア）
    globalParameter_->CreateGroup(rankGroupName_);

    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
    globalParameter_->SyncParamForGroup(rankGroupName_);

    auto* info   = GameResultInfo::GetInstance();
    currentItem_ = 0;

    items_[0].Init("ResultUI/EnemyKillCountText.dds", "ResultUI/ResultKillUnit.dds",
        info->GetTotalKillCount(), survivalCfg_);
    items_[1].Init("ResultUI/SurvivalTimeCount.dds", "ResultUI/SurvivalTimeCountUnit.dds",
        info->GetSurvivalTimeSec(), killCfg_);

    int32_t rank = info->GetRank(rankScoreForA_, rankScoreForS_);
    rankSprite_.Init(rank, rankCfg_);

    items_[0].Start();

    sprite_.reset(KetaEngine::Sprite::Create("ResultUI/ResultPresent.dds"));
    sprite_->SetIsDraw(true);
}

void GameResultUI::Update() {
    float dt = KetaEngine::Frame::DeltaTime();

    items_[0].SetConfig(survivalCfg_);
    items_[1].SetConfig(killCfg_);

    for (auto& item : items_) {
        item.Update(dt);
    }
    rankSprite_.Update(dt);

    // 現在アイテムが完了したら次へ進める
    if (currentItem_ < static_cast<int32_t>(items_.size())) {
        if (items_[currentItem_].IsFinished()) {
            ++currentItem_;
            if (currentItem_ < static_cast<int32_t>(items_.size())) {
                items_[currentItem_].Start();
            } else {
                // 全item完了 → ランクスプライトを開始
                rankSprite_.Start();
            }
        }
    }
}

void GameResultUI::RegisterParams() {
    // ① 生き残った時間
    globalParameter_->Regist(groupName_, "Survival_basePos",        &survivalCfg_.basePos);
    globalParameter_->Regist(groupName_, "Survival_labelOffset",    &survivalCfg_.labelOffset);
    globalParameter_->Regist(groupName_, "Survival_numberOffset",   &survivalCfg_.numberBaseOffset);
    globalParameter_->Regist(groupName_, "Survival_digitSpacing",   &survivalCfg_.digitSpacing);
    globalParameter_->Regist(groupName_, "Survival_unitOffset",     &survivalCfg_.unitOffset);
    globalParameter_->Regist(groupName_, "Survival_baseScale",      &survivalCfg_.baseScale);
    globalParameter_->Regist(groupName_, "Survival_digitScaleOffset", &survivalCfg_.digitScaleOffset);
    globalParameter_->Regist(groupName_, "Survival_startDelay",     &survivalCfg_.startDelay);

    // ② 敵を倒した数
    globalParameter_->Regist(groupName_, "Kill_basePos",            &killCfg_.basePos);
    globalParameter_->Regist(groupName_, "Kill_labelOffset",        &killCfg_.labelOffset);
    globalParameter_->Regist(groupName_, "Kill_numberOffset",       &killCfg_.numberBaseOffset);
    globalParameter_->Regist(groupName_, "Kill_digitSpacing",       &killCfg_.digitSpacing);
    globalParameter_->Regist(groupName_, "Kill_unitOffset",         &killCfg_.unitOffset);
    globalParameter_->Regist(groupName_, "Kill_baseScale",          &killCfg_.baseScale);
    globalParameter_->Regist(groupName_, "Kill_digitScaleOffset",   &killCfg_.digitScaleOffset);
    globalParameter_->Regist(groupName_, "Kill_startDelay",         &killCfg_.startDelay);

    // ③ ランク
    globalParameter_->Regist(groupName_, "Rank_basePos",            &rankCfg_.basePos);
    globalParameter_->Regist(groupName_, "Rank_baseScale",          &rankCfg_.baseScale);
    globalParameter_->Regist(groupName_, "Rank_startDelay",         &rankCfg_.startDelay);

    // ランク閾値（RankThresholdグループ）
    globalParameter_->Regist(rankGroupName_, "ScoreForA", &rankScoreForA_);
    globalParameter_->Regist(rankGroupName_, "ScoreForS", &rankScoreForS_);
}

void GameResultUI::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        if (ImGui::TreeNode("生き残った時間")) {
            ImGui::DragFloat2("BasePos",          &survivalCfg_.basePos.x,          1.f);
            ImGui::DragFloat2("LabelOffset",      &survivalCfg_.labelOffset.x,      1.f);
            ImGui::DragFloat2("NumberOffset",     &survivalCfg_.numberBaseOffset.x, 1.f);
            ImGui::DragFloat ("DigitSpacing",     &survivalCfg_.digitSpacing,       1.f);
            ImGui::DragFloat2("UnitOffset",       &survivalCfg_.unitOffset.x,       1.f);
            ImGui::DragFloat2("BaseScale",        &survivalCfg_.baseScale.x,        0.01f);
            ImGui::DragFloat2("DigitScaleOffset", &survivalCfg_.digitScaleOffset.x, 0.01f);
            ImGui::DragFloat ("StartDelay",       &survivalCfg_.startDelay,         0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("敵を倒した数")) {
            ImGui::DragFloat2("BasePos",          &killCfg_.basePos.x,          1.f);
            ImGui::DragFloat2("LabelOffset",      &killCfg_.labelOffset.x,      1.f);
            ImGui::DragFloat2("NumberOffset",     &killCfg_.numberBaseOffset.x, 1.f);
            ImGui::DragFloat ("DigitSpacing",     &killCfg_.digitSpacing,       1.f);
            ImGui::DragFloat2("UnitOffset",       &killCfg_.unitOffset.x,       1.f);
            ImGui::DragFloat2("BaseScale",        &killCfg_.baseScale.x,        0.01f);
            ImGui::DragFloat2("DigitScaleOffset", &killCfg_.digitScaleOffset.x, 0.01f);
            ImGui::DragFloat ("StartDelay",       &killCfg_.startDelay,         0.01f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("ランク (B/A/S)")) {
            ImGui::DragFloat2("BasePos",    &rankCfg_.basePos.x,   1.f);
            ImGui::DragFloat2("BaseScale",  &rankCfg_.baseScale.x, 0.01f);
            ImGui::DragFloat ("StartDelay", &rankCfg_.startDelay,  0.01f);
            ImGui::TreePop();
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader(rankGroupName_.c_str())) {
        ImGui::PushID(rankGroupName_.c_str());
        ImGui::DragInt("ScoreForA (BからA)", &rankScoreForA_, 1);
        ImGui::DragInt("ScoreForS (AからS)", &rankScoreForS_, 1);
        globalParameter_->ParamSaveForImGui(rankGroupName_);
        globalParameter_->ParamLoadForImGui(rankGroupName_);
        ImGui::PopID();
    }
#endif
}
