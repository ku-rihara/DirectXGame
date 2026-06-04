#include "GameResultUI.h"
#include "Frame/Frame.h"
#include "ResultObj/GameResultInfo.h"
#include <imgui.h>

GameResultUI::~GameResultUI() {
    if (globalParameter_) {
        globalParameter_->ClearRegistersForGroup(groupName_);
    }
}

void GameResultUI::Init() {

    // グローバルパラメータの登録
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    globalParameter_->ClearRegistersForGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    auto* resultInfo   = GameResultInfo::GetInstance();
    currentItem_ = 0;

    // テキストと単位のスプライトの初期化
    items_[0].Init("ResultUI/EnemyKillCountText.dds", "ResultUI/ResultKillUnit.dds",
        resultInfo->GetTotalKillCount(), survivalCfg_);
    items_[1].Init("ResultUI/SurvivalTimeCount.dds", "ResultUI/SurvivalTimeCountUnit.dds",
        resultInfo->GetSurvivalTimeSec(), killCfg_);

    //  ランクラベルの初期化
    rankLabelCfg_.digitCount = 0;
    items_[2].Init("ResultUI/RankText.dds", "", 0, rankLabelCfg_);

    // ランクの初期化
    int32_t rank = resultInfo->GetRank(rankScoreForA_, rankScoreForS_);
    rankSprite_.Init(rank, rankCfg_);

    // キルカウントからアニメーション開始
    items_[0].Start();

    // 結果発表スプライトの初期化
    sprite_.reset(KetaEngine::Sprite::Create("ResultUI/ResultPresent.dds"));
    sprite_->SetIsDraw(true);
}

void GameResultUI::Update() {
    float dt = KetaEngine::Frame::DeltaTime();

    // 各項目の設定を更新
    items_[0].SetConfig(survivalCfg_);
    items_[1].SetConfig(killCfg_);
    items_[2].SetConfig(rankLabelCfg_);

    // アニメーションの更新
    for (auto& item : items_) {
        item.Update(dt);
    }

    // ランクスプライトの更新
    rankSprite_.SetConfig(rankCfg_);
    rankSprite_.Update(dt);

    // 現在の項目が終了しているか確認
    if (currentItem_ < static_cast<int32_t>(items_.size())) {

        if (items_[currentItem_].IsFinished()) {
            // 次の項目へ加算
            ++currentItem_;

            // 次の項目を開始のアニメーション開始
            if (currentItem_ < static_cast<int32_t>(items_.size())) {
                items_[currentItem_].Start();
            } else {
                // 全項目表示後にランクスプライトのアニメーション開始
                rankSprite_.Start();
            }
        }
    }
}

void GameResultUI::RegisterParams() {
    struct Entry { 
        const char* prefix; 
    ResultUIItem::Config* cfg; 
    };

    const std::array<Entry, 3> items = {{
        { "Survival",  &survivalCfg_  },
        { "Kill",      &killCfg_      },
        { "RankLabel", &rankLabelCfg_ },
    }};

    for (auto& [prefix, cfg] : items) {
        std::string p = prefix;
        globalParameter_->Regist(groupName_, p + "_basePos",          &cfg->basePos);
        globalParameter_->Regist(groupName_, p + "_labelOffset",      &cfg->labelOffset);
        globalParameter_->Regist(groupName_, p + "_numberOffset",     &cfg->numberBaseOffset);
        globalParameter_->Regist(groupName_, p + "_digitSpacing",     &cfg->digitSpacing);
        globalParameter_->Regist(groupName_, p + "_unitOffset",       &cfg->unitOffset);
        globalParameter_->Regist(groupName_, p + "_baseScale",        &cfg->baseScale);
        globalParameter_->Regist(groupName_, p + "_digitScaleOffset", &cfg->digitScaleOffset);
        globalParameter_->Regist(groupName_, p + "_startDelay",       &cfg->startDelay);
    }

    // ランクバッジ
    globalParameter_->Regist(groupName_, "Rank_basePos",    &rankCfg_.basePos);
    globalParameter_->Regist(groupName_, "Rank_baseScale",  &rankCfg_.baseScale);
    globalParameter_->Regist(groupName_, "Rank_startDelay", &rankCfg_.startDelay);

    // ランク閾値
    globalParameter_->Regist(groupName_, "ScoreForA", &rankScoreForA_);
    globalParameter_->Regist(groupName_, "ScoreForS", &rankScoreForS_);
}

void GameResultUI::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        struct Entry { const char* label; ResultUIItem::Config* cfg; };
        const std::array<Entry, 3> items = {{
            { "生き残った時間", &survivalCfg_  },
            { "敵を倒した数",   &killCfg_      },
            { "ランクラベル",   &rankLabelCfg_ },
        }};

        for (auto& [label, cfg] : items) {
            if (ImGui::TreeNode(label)) {
                ImGui::DragFloat2("BasePos",          &cfg->basePos.x,          1.f);
                ImGui::DragFloat2("LabelOffset",      &cfg->labelOffset.x,      1.f);
                ImGui::DragFloat2("NumberOffset",     &cfg->numberBaseOffset.x, 1.f);
                ImGui::DragFloat ("DigitSpacing",     &cfg->digitSpacing,       1.f);
                ImGui::DragFloat2("UnitOffset",       &cfg->unitOffset.x,       1.f);
                ImGui::DragFloat2("BaseScale",        &cfg->baseScale.x,        0.01f);
                ImGui::DragFloat2("DigitScaleOffset", &cfg->digitScaleOffset.x, 0.01f);
                ImGui::DragFloat ("StartDelay",       &cfg->startDelay,         0.01f);
                ImGui::TreePop();
            }
        }

        if (ImGui::TreeNode("ランクバッジ (B/A/S)")) {
            ImGui::DragFloat2("BasePos",    &rankCfg_.basePos.x,   1.f);
            ImGui::DragFloat2("BaseScale",  &rankCfg_.baseScale.x, 0.01f);
            ImGui::DragFloat ("StartDelay", &rankCfg_.startDelay,  0.01f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("ランク閾値")) {
            ImGui::DragInt("ScoreForA (BからA)", &rankScoreForA_, 1);
            ImGui::DragInt("ScoreForS (AからS)", &rankScoreForS_, 1);
            ImGui::TreePop();
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
#endif
}
