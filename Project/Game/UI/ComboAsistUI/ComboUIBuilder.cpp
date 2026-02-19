#include "ComboUIBuilder.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"

///==========================================================
/// 全条件のパス構築＆UI生成
///==========================================================
void ComboUIBuilder::BuildAllConditions(
    PlayerComboAttackController* attackController,
    const LayoutParam& layoutParam,
    std::map<PlayerComboAttackData::TriggerCondition, ConditionUIData>& outConditionDataMap,
    std::vector<PlayerComboAttackData::TriggerCondition>& outAvailableConditions,
    PlayerComboAttackData::TriggerCondition& currentCondition) {

    using TC = PlayerComboAttackData::TriggerCondition;
    outConditionDataMap.clear();
    outAvailableConditions.clear();

    // コンディションごとに構築する
    for (TC cond : {TC::GROUND, TC::AIR, TC::DASH}) {
        ConditionUIData data;
        data.pathBuilder.Build(attackController, cond);
        if (!data.pathBuilder.GetXGroup().mainPath.steps.empty() ||
            !data.pathBuilder.GetYGroup().mainPath.steps.empty()) {
            CreateConditionUI(data, layoutParam);
            outConditionDataMap[cond] = std::move(data);
            outAvailableConditions.push_back(cond);
        }
    }

    if (outConditionDataMap.count(currentCondition) == 0 && !outAvailableConditions.empty()) {
        currentCondition = outAvailableConditions[0];
    }
}

///==========================================================
/// 条件別UI生成
///==========================================================
void ComboUIBuilder::CreateConditionUI(ConditionUIData& conditionData, const LayoutParam& layoutParam) {
    int32_t currentRow = 0;
    CreateGroupUI(conditionData.pathBuilder.GetXGroup(), conditionData.xUIGroup, &currentRow, layoutParam);
    CreateGroupUI(conditionData.pathBuilder.GetYGroup(), conditionData.yUIGroup, &currentRow, layoutParam);
}

///==========================================================
/// グループのUI生成
///==========================================================
void ComboUIBuilder::CreateGroupUI(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    int32_t* currentRow,
    const LayoutParam& layoutParam) {

    if (pathGroup.mainPath.steps.empty()) {
        return;
    }

    int32_t mainRow = *currentRow;

    // メインパスの列マッピング構築
    std::vector<int32_t> mainColMap(pathGroup.mainPath.steps.size(), -1);
    int32_t displayCol = 0;
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        if (i == 0 || !pathGroup.mainPath.steps[i - 1].isAutoAdvance) {
            mainColMap[i] = displayCol++;
        }
    }

    // メインパスのボタン
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        if (mainColMap[i] < 0) {
            continue;
        }
        const auto& step = pathGroup.mainPath.steps[i];
        auto btn         = std::make_unique<ComboAsistButtonUI>();
        btn->Init(step.gamepadButton, step.isUnlocked, mainRow, mainColMap[i], layoutParam);
        uiGroup.mainButtonUIs.push_back(std::move(btn));
    }

    // メインパスの矢印
    int32_t prevCol = -1;
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        if (mainColMap[i] < 0) {
            continue;
        }
        if (prevCol >= 0) {
            auto arrow = std::make_unique<ComboAsistArrowUI>();
            arrow->Init(prevCol, mainRow, mainColMap[i], mainRow, layoutParam);
            uiGroup.mainArrowUIs.push_back(std::move(arrow));
        }
        prevCol = mainColMap[i];
    }

    (*currentRow)++;

    // 分岐パスのUI
    uiGroup.branchButtonUIs.resize(pathGroup.branches.size());
    uiGroup.branchInnerArrowUIs.resize(pathGroup.branches.size());

    for (size_t branchIndex = 0; branchIndex < pathGroup.branches.size(); ++branchIndex) {
        const auto& branchInfo = pathGroup.branches[branchIndex];
        int32_t branchRow      = *currentRow;
        auto& buttons          = uiGroup.branchButtonUIs[branchIndex];
        auto& innerArrows      = uiGroup.branchInnerArrowUIs[branchIndex];

        // 共通部分の最後の表示列
        int32_t lastSharedCol = -1;
        for (int32_t j = branchInfo.divergeIndex - 1; j >= 0; --j) {
            if (j < static_cast<int32_t>(mainColMap.size()) && mainColMap[j] >= 0) {
                lastSharedCol = mainColMap[j];
                break;
            }
        }

        // 分岐部分の列マッピング
        std::vector<int32_t> branchColMap(branchInfo.path.steps.size(), -1);
        int32_t branchCol = lastSharedCol + 1;
        for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
            if (i == 0 || !branchInfo.path.steps[i - 1].isAutoAdvance) {
                branchColMap[i] = branchCol++;
            }
        }

        // 分岐元→分岐先の斜め矢印
        if (branchInfo.divergeIndex > 0 &&
            branchInfo.divergeIndex < static_cast<int32_t>(branchInfo.path.steps.size()) &&
            lastSharedCol >= 0 && branchColMap[branchInfo.divergeIndex] >= 0) {
            auto arrow = std::make_unique<ComboAsistArrowUI>();
            arrow->Init(lastSharedCol, mainRow, branchColMap[branchInfo.divergeIndex], branchRow, layoutParam);
            uiGroup.branchArrowUIs.push_back(std::move(arrow));
        }

        // 分岐パスのボタン
        for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
            if (branchColMap[i] < 0) {
                continue;
            }
            auto btn = std::make_unique<ComboAsistButtonUI>();
            btn->Init(branchInfo.path.steps[i].gamepadButton, branchInfo.path.steps[i].isUnlocked, branchRow, branchColMap[i], layoutParam);
            buttons.push_back(std::move(btn));
        }

        // 分岐パス内の矢印
        int32_t prevBCol = -1;
        for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
            if (branchColMap[i] < 0) {
                continue;
            }
            if (prevBCol >= 0) {
                auto arrow = std::make_unique<ComboAsistArrowUI>();
                arrow->Init(prevBCol, branchRow, branchColMap[i], branchRow, layoutParam);
                innerArrows.push_back(std::move(arrow));
            }
            prevBCol = branchColMap[i];
        }

        (*currentRow)++;
    }
}

///==========================================================
/// ヘルパー：グループ全UI要素に操作を適用
///==========================================================
void ComboUIBuilder::ApplyToGroup(ComboUIGroup& uiGroup, const std::function<void(BaseComboAsistUI&)>& func) {
    for (auto& ui : uiGroup.mainButtonUIs) {
        func(*ui);
    }
    for (auto& ui : uiGroup.mainArrowUIs) {
        func(*ui);
    }
    for (auto& ui : uiGroup.branchArrowUIs) {
        func(*ui);
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& ui : buttonRow) {
            func(*ui);
        }
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& ui : arrowRow) {
            func(*ui);
        }
    }
}

///==========================================================
/// ヘルパー：条件データ x/y 両グループに操作を適用
///==========================================================
void ComboUIBuilder::ApplyToCondition(ConditionUIData& conditionData, const std::function<void(BaseComboAsistUI&)>& func) {
    ApplyToGroup(conditionData.xUIGroup, func);
    ApplyToGroup(conditionData.yUIGroup, func);
}

///==========================================================
/// ヘルパー：パスステップとUIボタンのペアに操作を適用
///==========================================================
void ComboUIBuilder::ForEachStepButton(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::function<void(const ComboPathBuilder::ComboStep&, ComboAsistButtonUI&)>& func) {

    // メインパス
    size_t idx = 0;
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        if (i > 0 && pathGroup.mainPath.steps[i - 1].isAutoAdvance) {
            continue;
        }
        if (idx < uiGroup.mainButtonUIs.size()) {
            func(pathGroup.mainPath.steps[i], *uiGroup.mainButtonUIs[idx]);
        }
        idx++;
    }

    // 分岐パス
    for (size_t branchIndex = 0; branchIndex < pathGroup.branches.size() && branchIndex < uiGroup.branchButtonUIs.size(); ++branchIndex) {
        const auto& branchInfo = pathGroup.branches[branchIndex];
        auto& branchButtons    = uiGroup.branchButtonUIs[branchIndex];
        size_t buttonIndex     = 0;
        for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
            if (i > 0 && branchInfo.path.steps[i - 1].isAutoAdvance) {
                continue;
            }
            if (buttonIndex < branchButtons.size()) {
                func(branchInfo.path.steps[i], *branchButtons[buttonIndex]);
            }
            buttonIndex++;
        }
    }
}
