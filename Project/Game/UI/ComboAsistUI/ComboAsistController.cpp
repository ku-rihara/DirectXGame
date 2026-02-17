#include "ComboAsistController.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "input/InputData.h"
#include "Player/Player.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include <algorithm>
#include <cmath>
#include <imgui.h>

void ComboAsistController::Init() {
    if (!pAttackController_) {
        return;
    }

    // GlobalParameter初期化
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // 全条件のパス構築＆UI生成
    currentCondition_ = PlayerComboAttackData::TriggerCondition::GROUND;
    BuildAllConditions();

    // スライドイージング初期化
    slideInEasing_.Init("ComboAsistSlideIn.json");
    slideInEasing_.SetAdaptValue(&slideOffsetX_);
    slideInEasing_.Reset();

    slideOutEasing_.Init("ComboAsistSlideOut.json");
    slideOutEasing_.SetAdaptValue(&slideOffsetX_);
    slideOutEasing_.Reset();

    activeSlideEasing_ = nullptr;
    isVisible_ = false;
    isSliding_ = false;

    // 列オーバーフローイージング初期化
    columnOverflowEasing_.Init("ComboAsistColumnOverflow.json");
    columnOverflowEasing_.SetAdaptValue(&overflowScale_);
    columnOverflowEasing_.Reset();
    isAutoSwitchedCondition_ = false;

    // 初期条件以外は非表示
    for (auto& [cond, data] : conditionDataMap_) {
        if (cond != currentCondition_) {
            SetConditionVisible(data, false);
        }
    }

    // 初期状態は画面外に隠す
    slideOffsetX_ = slideInEasing_.GetStartValue();
    ApplySlideOffset();
}

void ComboAsistController::Update() {
    SyncUnlockStates();
    UpdateComboState();
    CheckToggleInput();
    CheckConditionSwitchInput();
    CheckAutoConditionSwitch();
    UpdateSlide(KetaEngine::Frame::DeltaTime());
    ApplySlideOffset();
    CheckColumnOverflow();
    UpdateVisibility();

    // 現在の条件のUIのみLerp更新
    auto it = conditionDataMap_.find(currentCondition_);
    if (it == conditionDataMap_.end()) {
        return;
    }
    auto& curData = it->second;

    auto updateGroup = [](ComboUIGroup& uiGroup) {
        for (auto& btn : uiGroup.mainButtonUIs) {
            btn->Update();
        }
        for (auto& arrow : uiGroup.mainArrowUIs) {
            arrow->Update();
        }
        for (auto& arrow : uiGroup.branchArrowUIs) {
            arrow->Update();
        }
        for (auto& buttons : uiGroup.branchButtonUIs) {
            for (auto& btn : buttons) {
                btn->Update();
            }
        }
        for (auto& arrows : uiGroup.branchInnerArrowUIs) {
            for (auto& arrow : arrows) {
                arrow->Update();
            }
        }
    };
    updateGroup(curData.xUIGroup);
    updateGroup(curData.yUIGroup);
}

///==========================================================
/// レイアウト情報の構築
///==========================================================
LayoutParam ComboAsistController::MakeLayoutParam() const {
    LayoutParam lp;
    lp.basePosition  = basePosition_;
    lp.arrowOffset   = arrowOffset_;
    lp.columnSpacing = columnSpacing_;
    lp.rowSpacing    = rowSpacing_;
    lp.branchYOffset = branchYOffset_;
    lp.buttonScale   = buttonScale_;
    lp.arrowScale    = arrowScale_;
    return lp;
}

///==========================================================
/// 全条件のパス構築＆UI生成
///==========================================================
void ComboAsistController::BuildAllConditions() {
    using TC = PlayerComboAttackData::TriggerCondition;

    conditionDataMap_.clear();
    availableConditions_.clear();

    const TC conditions[] = { TC::GROUND, TC::AIR, TC::DASH, TC::BOTH, TC::JUSTACTION };

    for (TC cond : conditions) {
        ConditionUIData data;
        data.pathBuilder.Build(pAttackController_, cond);

        bool hasX = !data.pathBuilder.GetXGroup().mainPath.steps.empty();
        bool hasY = !data.pathBuilder.GetYGroup().mainPath.steps.empty();
        if (hasX || hasY) {
            CreateConditionUI(data);
            conditionDataMap_[cond] = std::move(data);
            availableConditions_.push_back(cond);
        }
    }

    // 現在の条件が利用不可なら最初の利用可能条件に切替
    if (conditionDataMap_.count(currentCondition_) == 0 && !availableConditions_.empty()) {
        currentCondition_ = availableConditions_[0];
    }
}

///==========================================================
/// 条件別UI生成
///==========================================================
void ComboAsistController::CreateConditionUI(ConditionUIData& data) {
    int32_t currentRow = 0;
    CreateGroupUI(data.pathBuilder.GetXGroup(), data.xUIGroup, &currentRow);
    CreateGroupUI(data.pathBuilder.GetYGroup(), data.yUIGroup, &currentRow);
}

///==========================================================
/// グループのUI生成
///==========================================================
void ComboAsistController::CreateGroupUI(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    int32_t* currentRow) {

    if (pathGroup.mainPath.steps.empty()) {
        return;
    }

    LayoutParam lp = MakeLayoutParam();
    int32_t mainRow = *currentRow;

    // --- メインパスの列マッピング構築 ---
    // step[i] は i==0 または step[i-1].isAutoAdvance==false の場合のみ表示
    std::vector<int32_t> mainColumnMap(pathGroup.mainPath.steps.size(), -1);
    int32_t displayCol = 0;
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        bool needsButton = (i == 0) || !pathGroup.mainPath.steps[i - 1].isAutoAdvance;
        if (needsButton) {
            mainColumnMap[i] = displayCol++;
        }
    }

    // --- メインパスのボタン ---
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        if (mainColumnMap[i] < 0) {
            continue;
        }
        const auto& step = pathGroup.mainPath.steps[i];
        auto btn = std::make_unique<ComboAsistButtonUI>();
        btn->Init(step.gamepadButton, step.isUnlocked, mainRow, mainColumnMap[i], lp);
        uiGroup.mainButtonUIs.push_back(std::move(btn));
    }

    // --- メインパスの矢印（表示ステップ間のみ） ---
    int32_t prevVisibleCol = -1;
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        if (mainColumnMap[i] < 0) {
            continue;
        }
        if (prevVisibleCol >= 0) {
            auto arrow = std::make_unique<ComboAsistArrowUI>();
            arrow->Init(prevVisibleCol, mainRow, mainColumnMap[i], mainRow, lp);
            uiGroup.mainArrowUIs.push_back(std::move(arrow));
        }
        prevVisibleCol = mainColumnMap[i];
    }

    (*currentRow)++;

    // --- 分岐パスのUI ---
    uiGroup.branchButtonUIs.resize(pathGroup.branches.size());
    uiGroup.branchInnerArrowUIs.resize(pathGroup.branches.size());

    for (size_t bIdx = 0; bIdx < pathGroup.branches.size(); ++bIdx) {
        const auto& branchInfo = pathGroup.branches[bIdx];
        int32_t branchRow      = *currentRow;
        auto& buttons          = uiGroup.branchButtonUIs[bIdx];
        auto& innerArrows      = uiGroup.branchInnerArrowUIs[bIdx];

        // --- 分岐パスの列マッピング構築 ---
        // 共通部分（divergeIndex前）のうち最後の表示列を求め、分岐部分はそこから続ける
        int32_t lastSharedCol = -1;
        for (int32_t j = branchInfo.divergeIndex - 1; j >= 0; --j) {
            if (j < static_cast<int32_t>(mainColumnMap.size()) && mainColumnMap[j] >= 0) {
                lastSharedCol = mainColumnMap[j];
                break;
            }
        }

        // 分岐部分の列マッピング
        std::vector<int32_t> branchColumnMap(branchInfo.path.steps.size(), -1);
        int32_t branchCol = lastSharedCol + 1;
        for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
            // divergeIndex == 0 の場合は最初のステップなので必ず表示
            bool needsButton = (i == 0) || !branchInfo.path.steps[i - 1].isAutoAdvance;
            if (needsButton) {
                branchColumnMap[i] = branchCol++;
            }
        }

        // 分岐元から分岐先への斜め矢印
        if (branchInfo.divergeIndex > 0 &&
            branchInfo.divergeIndex < static_cast<int32_t>(branchInfo.path.steps.size()) &&
            lastSharedCol >= 0 &&
            branchColumnMap[branchInfo.divergeIndex] >= 0) {
            auto branchArrow = std::make_unique<ComboAsistArrowUI>();
            branchArrow->Init(lastSharedCol, mainRow, branchColumnMap[branchInfo.divergeIndex], branchRow, lp);
            uiGroup.branchArrowUIs.push_back(std::move(branchArrow));
        }

        // 分岐パスのボタン
        for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
            if (branchColumnMap[i] < 0) {
                continue;
            }
            const auto& step = branchInfo.path.steps[i];
            auto btn = std::make_unique<ComboAsistButtonUI>();
            btn->Init(step.gamepadButton, step.isUnlocked, branchRow, branchColumnMap[i], lp);
            buttons.push_back(std::move(btn));
        }

        // 分岐パス内の矢印（表示ステップ間のみ）
        int32_t prevBranchVisibleCol = -1;
        for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
            if (branchColumnMap[i] < 0) {
                continue;
            }
            if (prevBranchVisibleCol >= 0) {
                auto arrow = std::make_unique<ComboAsistArrowUI>();
                arrow->Init(prevBranchVisibleCol, branchRow, branchColumnMap[i], branchRow, lp);
                innerArrows.push_back(std::move(arrow));
            }
            prevBranchVisibleCol = branchColumnMap[i];
        }

        (*currentRow)++;
    }
}

///==========================================================
/// スライドイン開始
///==========================================================
void ComboAsistController::StartSlideIn() {
    activeSlideEasing_ = &slideInEasing_;
    activeSlideEasing_->Reset();
    isSliding_ = true;
    isVisible_ = true;
}

///==========================================================
/// スライドアウト開始
///==========================================================
void ComboAsistController::StartSlideOut() {
    activeSlideEasing_ = &slideOutEasing_;
    activeSlideEasing_->Reset();
    isSliding_ = true;
}

///==========================================================
/// スライド更新
///==========================================================
void ComboAsistController::UpdateSlide(float deltaTime) {
    if (!isSliding_ || !activeSlideEasing_) {
        return;
    }
    activeSlideEasing_->Update(deltaTime);
    if (activeSlideEasing_->IsFinished()) {
        isSliding_ = false;
        // スライドアウト完了時に非表示
        if (activeSlideEasing_ == &slideOutEasing_) {
            isVisible_ = false;
        }
    }
}

///==========================================================
/// スライドオフセット適用（現在の条件のみ）
///==========================================================
void ComboAsistController::ApplySlideOffset() {
    auto it = conditionDataMap_.find(currentCondition_);
    if (it == conditionDataMap_.end()) {
        return;
    }
    auto& curData = it->second;

    auto applyToGroup = [&](ComboUIGroup& uiGroup) {
        for (auto& btn : uiGroup.mainButtonUIs) {
            btn->ApplySlideOffset(slideOffsetX_);
        }
        for (auto& arrow : uiGroup.mainArrowUIs) {
            arrow->ApplySlideOffset(slideOffsetX_);
        }
        for (auto& arrow : uiGroup.branchArrowUIs) {
            arrow->ApplySlideOffset(slideOffsetX_);
        }
        for (auto& buttons : uiGroup.branchButtonUIs) {
            for (auto& btn : buttons) {
                btn->ApplySlideOffset(slideOffsetX_);
            }
        }
        for (auto& arrows : uiGroup.branchInnerArrowUIs) {
            for (auto& arrow : arrows) {
                arrow->ApplySlideOffset(slideOffsetX_);
            }
        }
    };

    applyToGroup(curData.xUIGroup);
    applyToGroup(curData.yUIGroup);
}

///==========================================================
/// トグル入力チェック
///==========================================================
void ComboAsistController::CheckToggleInput() {
    if (KetaEngine::Input::IsTriggerPad(0, GamepadButton::DPadRight)) {
        if (isVisible_) {
            StartSlideOut();
        } else {
            StartSlideIn();
        }
    }
}

///==========================================================
/// ロック状態の同期
///==========================================================
void ComboAsistController::SyncUnlockStates() {
    if (!pAttackController_) {
        return;
    }

    // 全条件のロック状態を同期
    for (auto& [cond, data] : conditionDataMap_) {
        SyncGroupUnlockStates(data.pathBuilder.GetXGroup(), data.xUIGroup);
        SyncGroupUnlockStates(data.pathBuilder.GetYGroup(), data.yUIGroup);
    }
}

void ComboAsistController::SyncGroupUnlockStates(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup) {

    // メインパス（自動遷移ステップはUI未生成なのでスキップ）
    size_t btnIdx = 0;
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        bool needsButton = (i == 0) || !pathGroup.mainPath.steps[i - 1].isAutoAdvance;
        if (!needsButton) {
            continue;
        }
        auto* attack = pAttackController_->GetAttackByName(pathGroup.mainPath.steps[i].attackName);
        if (attack && btnIdx < uiGroup.mainButtonUIs.size()) {
            uiGroup.mainButtonUIs[btnIdx]->SetUnlocked(attack->GetAttackParam().isUnlocked);
        }
        btnIdx++;
    }

    // 分岐パス
    for (size_t bIdx = 0; bIdx < pathGroup.branches.size() && bIdx < uiGroup.branchButtonUIs.size(); ++bIdx) {
        const auto& branch = pathGroup.branches[bIdx];
        auto& buttons      = uiGroup.branchButtonUIs[bIdx];

        size_t branchBtnIdx = 0;
        for (size_t i = branch.divergeIndex; i < branch.path.steps.size(); ++i) {
            bool needsButton = (i == 0) || !branch.path.steps[i - 1].isAutoAdvance;
            if (!needsButton) {
                continue;
            }
            auto* attack = pAttackController_->GetAttackByName(branch.path.steps[i].attackName);
            if (attack && branchBtnIdx < buttons.size()) {
                buttons[branchBtnIdx]->SetUnlocked(attack->GetAttackParam().isUnlocked);
            }
            branchBtnIdx++;
        }
    }
}

///==========================================================
/// 攻撃発動演出の更新
///==========================================================
void ComboAsistController::UpdateComboState() {
    if (!pPlayer_) {
        return;
    }

    auto it = conditionDataMap_.find(currentCondition_);
    if (it == conditionDataMap_.end()) {
        return;
    }
    auto& curData = it->second;

    auto* comboBehavior = pPlayer_->GetComboBehavior();
    if (!comboBehavior) {
        return;
    }

    const std::string& behaviorName = comboBehavior->GetName();

    if (behaviorName == "ComboAttackRoot") {
        // ComboAttackRootに戻ったら全リセット
        if (!playedAttacks_.empty()) {
            playedAttacks_.clear();
            SetGroupActiveOutLines(curData.pathBuilder.GetYGroup(), curData.yUIGroup, playedAttacks_);
            SetGroupActiveOutLines(curData.pathBuilder.GetXGroup(), curData.xUIGroup, playedAttacks_);
        }

        // 列シフトをリセット（元の位置に戻す）
        if (curData.columnShiftAmount != 0) {
            ShiftAllColumns(curData.columnShiftAmount);
        }

        // 自動切替されたAIRをGROUNDに戻す（手動切替は戻さない）
        if (isAutoSwitchedCondition_) {
            isAutoSwitchedCondition_ = false;
            SwitchCondition(PlayerComboAttackData::TriggerCondition::GROUND);
        }
    } else {
        // 新たな攻撃が発動した瞬間にPushScalingアニメ再生
        bool isNewAttack = (behaviorName != prevBehaviorName_);
        playedAttacks_.insert(behaviorName);
        SetGroupActiveOutLines(curData.pathBuilder.GetYGroup(), curData.yUIGroup, playedAttacks_);
        SetGroupActiveOutLines(curData.pathBuilder.GetXGroup(), curData.xUIGroup, playedAttacks_);

        if (isNewAttack) {
            PlayPushScalingForAttack(curData.pathBuilder.GetYGroup(), curData.yUIGroup, behaviorName);
            PlayPushScalingForAttack(curData.pathBuilder.GetXGroup(), curData.xUIGroup, behaviorName);
        }
    }

    prevBehaviorName_ = behaviorName;
}

///==========================================================
/// グループ内ボタンのアクティブアウトライン設定
///==========================================================
void ComboAsistController::SetGroupActiveOutLines(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::unordered_set<std::string>& activeAttacks) {

    // メインパス（自動遷移ステップスキップ）
    size_t btnIdx = 0;
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        bool needsButton = (i == 0) || !pathGroup.mainPath.steps[i - 1].isAutoAdvance;
        if (!needsButton) {
            continue;
        }
        if (btnIdx < uiGroup.mainButtonUIs.size()) {
            bool active = activeAttacks.count(pathGroup.mainPath.steps[i].attackName) > 0;
            uiGroup.mainButtonUIs[btnIdx]->SetActiveOutLine(active);
        }
        btnIdx++;
    }

    // 分岐パス
    for (size_t bIdx = 0; bIdx < pathGroup.branches.size() && bIdx < uiGroup.branchButtonUIs.size(); ++bIdx) {
        const auto& branch = pathGroup.branches[bIdx];
        auto& buttons      = uiGroup.branchButtonUIs[bIdx];

        size_t branchBtnIdx = 0;
        for (size_t i = branch.divergeIndex; i < branch.path.steps.size(); ++i) {
            bool needsButton = (i == 0) || !branch.path.steps[i - 1].isAutoAdvance;
            if (!needsButton) {
                continue;
            }
            if (branchBtnIdx < buttons.size()) {
                bool active = activeAttacks.count(branch.path.steps[i].attackName) > 0;
                buttons[branchBtnIdx]->SetActiveOutLine(active);
            }
            branchBtnIdx++;
        }
    }
}

///==========================================================
/// 攻撃名に対応するボタンのPushScalingアニメーション再生
///==========================================================
void ComboAsistController::PlayPushScalingForAttack(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::string& attackName) {

    // メインパス（自動遷移ステップスキップ）
    size_t btnIdx = 0;
    for (size_t i = 0; i < pathGroup.mainPath.steps.size(); ++i) {
        bool needsButton = (i == 0) || !pathGroup.mainPath.steps[i - 1].isAutoAdvance;
        if (!needsButton) {
            continue;
        }
        if (pathGroup.mainPath.steps[i].attackName == attackName && btnIdx < uiGroup.mainButtonUIs.size()) {
            uiGroup.mainButtonUIs[btnIdx]->PlayPushScaling();
            return;
        }
        btnIdx++;
    }

    // 分岐パス
    for (size_t bIdx = 0; bIdx < pathGroup.branches.size() && bIdx < uiGroup.branchButtonUIs.size(); ++bIdx) {
        const auto& branch = pathGroup.branches[bIdx];
        auto& buttons      = uiGroup.branchButtonUIs[bIdx];

        size_t branchBtnIdx = 0;
        for (size_t i = branch.divergeIndex; i < branch.path.steps.size(); ++i) {
            bool needsButton = (i == 0) || !branch.path.steps[i - 1].isAutoAdvance;
            if (!needsButton) {
                continue;
            }
            if (branch.path.steps[i].attackName == attackName && branchBtnIdx < buttons.size()) {
                buttons[branchBtnIdx]->PlayPushScaling();
                return;
            }
            branchBtnIdx++;
        }
    }
}

///==========================================================
/// 発動条件切替入力チェック
///==========================================================
void ComboAsistController::CheckConditionSwitchInput() {
    if (KetaEngine::Input::IsTriggerPad(0, GamepadButton::DPadUp)) {
        if (availableConditions_.size() <= 1) {
            return;
        }

        // 現在の条件のインデックスを見つけて次へ循環
        auto found = std::find(availableConditions_.begin(), availableConditions_.end(), currentCondition_);
        size_t idx = (found != availableConditions_.end()) ? static_cast<size_t>(found - availableConditions_.begin()) : 0;
        size_t nextIdx = (idx + 1) % availableConditions_.size();

        isAutoSwitchedCondition_ = false; // 手動切替
        SwitchCondition(availableConditions_[nextIdx]);
    }
}

///==========================================================
/// ジャンプ攻撃自動切替チェック
///==========================================================
void ComboAsistController::CheckAutoConditionSwitch() {
    using TC = PlayerComboAttackData::TriggerCondition;
    if (!isVisible_ || !pPlayer_ || !pAttackController_) {
        return;
    }

    // GROUND表示中のみ自動切替
    if (currentCondition_ != TC::GROUND) {
        return;
    }

    auto* comboBehavior = pPlayer_->GetComboBehavior();
    if (!comboBehavior) {
        return;
    }

    const std::string& behaviorName = comboBehavior->GetName();
    if (behaviorName == "ComboAttackRoot") {
        return;
    }

    // 新たな攻撃が発動した瞬間のみチェック
    if (behaviorName == prevBehaviorName_) {
        return;
    }

    // 攻撃データを取得し、AIR条件なら自動切替
    auto* attack = pAttackController_->GetAttackByName(behaviorName);
    if (attack && attack->GetAttackParam().triggerParam.condition == TC::AIR) {
        if (conditionDataMap_.count(TC::AIR) > 0) {
            isAutoSwitchedCondition_ = true;
            SwitchCondition(TC::AIR);
        }
    }
}

///==========================================================
/// 条件切替（再ビルドなし・表示切替のみ）
///==========================================================
void ComboAsistController::SwitchCondition(PlayerComboAttackData::TriggerCondition condition) {
    if (conditionDataMap_.count(condition) == 0) {
        return;
    }
    if (condition == currentCondition_) {
        return;
    }

    // 旧条件を非表示
    if (conditionDataMap_.count(currentCondition_) > 0) {
        SetConditionVisible(conditionDataMap_[currentCondition_], false);
    }

    // 条件切替
    currentCondition_ = condition;
    auto& newData = conditionDataMap_[currentCondition_];

    // スライドオフセットを新条件のUIに適用
    ApplySlideOffset();

    // Lerpせず即座に位置を反映（非表示だった間のズレを防止）
    auto snapGroup = [](ComboUIGroup& uiGroup) {
        for (auto& btn : uiGroup.mainButtonUIs) { btn->SnapToTarget(); }
        for (auto& arrow : uiGroup.mainArrowUIs) { arrow->SnapToTarget(); }
        for (auto& arrow : uiGroup.branchArrowUIs) { arrow->SnapToTarget(); }
        for (auto& buttons : uiGroup.branchButtonUIs) {
            for (auto& btn : buttons) { btn->SnapToTarget(); }
        }
        for (auto& arrows : uiGroup.branchInnerArrowUIs) {
            for (auto& arrow : arrows) { arrow->SnapToTarget(); }
        }
    };
    snapGroup(newData.xUIGroup);
    snapGroup(newData.yUIGroup);

    // 表示範囲に基づいて表示更新
    UpdateVisibility();

    // 演出リセット
    playedAttacks_.clear();
}

///==========================================================
/// 条件データの全UI表示/非表示
///==========================================================
void ComboAsistController::SetConditionVisible(ConditionUIData& data, bool visible) {
    auto setGroupVisible = [visible](ComboUIGroup& uiGroup) {
        for (auto& btn : uiGroup.mainButtonUIs) { btn->SetVisible(visible); }
        for (auto& arrow : uiGroup.mainArrowUIs) { arrow->SetVisible(visible); }
        for (auto& arrow : uiGroup.branchArrowUIs) { arrow->SetVisible(visible); }
        for (auto& buttons : uiGroup.branchButtonUIs) {
            for (auto& btn : buttons) { btn->SetVisible(visible); }
        }
        for (auto& arrows : uiGroup.branchInnerArrowUIs) {
            for (auto& arrow : arrows) { arrow->SetVisible(visible); }
        }
    };
    setGroupVisible(data.xUIGroup);
    setGroupVisible(data.yUIGroup);
}

///==========================================================
/// 列オーバーフローチェック
///==========================================================
void ComboAsistController::CheckColumnOverflow() {
    if (!isVisible_) {
        return;
    }

    auto it = conditionDataMap_.find(currentCondition_);
    if (it == conditionDataMap_.end()) {
        return;
    }
    auto& curData = it->second;

    // オーバーフローイージング更新
    if (curData.isColumnShifting) {
        columnOverflowEasing_.Update(KetaEngine::Frame::DeltaTime());
        if (columnOverflowEasing_.IsFinished()) {
            curData.isColumnShifting = false;
            ShiftAllColumns(-1);
        }
        return;
    }

    // 現在のアクティブ攻撃のcolumnがmaxVisibleColumnを超えているかチェック
    auto checkGroup = [&](ComboUIGroup& uiGroup) {
        for (auto& btn : uiGroup.mainButtonUIs) {
            if (btn->GetState() != BaseComboAsistUI::AsistState::NONE &&
                btn->GetColumnNum() > maxVisibleColumn_) {
                // オーバーフロー開始
                curData.isColumnShifting = true;
                columnOverflowEasing_.Reset();

                // column 0 のボタンのスケールをイージングで0にする
                for (auto& b : uiGroup.mainButtonUIs) {
                    if (b->GetColumnNum() == 0) {
                        columnOverflowEasing_.SetAdaptValue(&overflowScale_);
                        break;
                    }
                }
                return true;
            }
        }
        return false;
    };

    checkGroup(curData.xUIGroup);
    checkGroup(curData.yUIGroup);
}

///==========================================================
/// 全列シフト
///==========================================================
void ComboAsistController::ShiftAllColumns(int32_t delta) {
    auto it = conditionDataMap_.find(currentCondition_);
    if (it == conditionDataMap_.end()) {
        return;
    }
    auto& curData = it->second;

    auto shiftGroup = [&](ComboUIGroup& uiGroup) {
        for (auto& btn : uiGroup.mainButtonUIs) {
            btn->SetRowColumn(btn->GetRowNum(), btn->GetColumnNum() + delta);
            btn->ApplyLayout();
        }
        for (auto& arrow : uiGroup.mainArrowUIs) {
            arrow->ShiftColumns(delta);
            arrow->ApplyLayout();
        }
        for (auto& arrow : uiGroup.branchArrowUIs) {
            arrow->ShiftColumns(delta);
            arrow->ApplyLayout();
        }
        for (auto& buttons : uiGroup.branchButtonUIs) {
            for (auto& btn : buttons) {
                btn->SetRowColumn(btn->GetRowNum(), btn->GetColumnNum() + delta);
                btn->ApplyLayout();
            }
        }
        for (auto& arrows : uiGroup.branchInnerArrowUIs) {
            for (auto& arrow : arrows) {
                arrow->ShiftColumns(delta);
                arrow->ApplyLayout();
            }
        }
    };

    shiftGroup(curData.xUIGroup);
    shiftGroup(curData.yUIGroup);
    curData.columnShiftAmount -= delta; // deltaが-1なら+1
}

///==========================================================
/// 表示範囲の更新
///==========================================================
void ComboAsistController::UpdateVisibility() {
    if (!isVisible_) {
        return;
    }

    auto it = conditionDataMap_.find(currentCondition_);
    if (it == conditionDataMap_.end()) {
        return;
    }
    auto& curData = it->second;

    auto updateGroupVisibility = [&](ComboUIGroup& uiGroup) {
        for (auto& btn : uiGroup.mainButtonUIs) {
            bool visible = btn->GetColumnNum() >= 0 &&
                           btn->GetColumnNum() <= maxVisibleColumn_ &&
                           btn->GetRowNum() >= 0 &&
                           btn->GetRowNum() <= maxVisibleRow_;
            btn->SetVisible(visible);
        }
        for (auto& arrow : uiGroup.mainArrowUIs) {
            bool visible = arrow->GetColumnNum() >= 0 &&
                           arrow->GetColumnNum() <= maxVisibleColumn_ &&
                           arrow->GetRowNum() >= 0 &&
                           arrow->GetRowNum() <= maxVisibleRow_;
            arrow->SetVisible(visible);
        }
        for (auto& arrow : uiGroup.branchArrowUIs) {
            bool visible = arrow->GetFromCol() >= 0 &&
                           arrow->GetToCol() <= maxVisibleColumn_ &&
                           arrow->GetFromRow() >= 0 &&
                           arrow->GetToRow() <= maxVisibleRow_;
            arrow->SetVisible(visible);
        }
        for (auto& buttons : uiGroup.branchButtonUIs) {
            for (auto& btn : buttons) {
                bool visible = btn->GetColumnNum() >= 0 &&
                               btn->GetColumnNum() <= maxVisibleColumn_ &&
                               btn->GetRowNum() >= 0 &&
                               btn->GetRowNum() <= maxVisibleRow_;
                btn->SetVisible(visible);
            }
        }
        for (auto& arrows : uiGroup.branchInnerArrowUIs) {
            for (auto& arrow : arrows) {
                bool visible = arrow->GetFromCol() >= 0 &&
                               arrow->GetToCol() <= maxVisibleColumn_ &&
                               arrow->GetFromRow() >= 0 &&
                               arrow->GetToRow() <= maxVisibleRow_;
                arrow->SetVisible(visible);
            }
        }
    };

    updateGroupVisibility(curData.xUIGroup);
    updateGroupVisibility(curData.yUIGroup);
}

///==========================================================
/// パラメータ登録
///==========================================================
void ComboAsistController::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePosition", &basePosition_);
    globalParameter_->Regist(groupName_, "arrowOffset", &arrowOffset_);
    globalParameter_->Regist(groupName_, "columnSpacing", &columnSpacing_);
    globalParameter_->Regist(groupName_, "rowSpacing", &rowSpacing_);
    globalParameter_->Regist(groupName_, "branchYOffset", &branchYOffset_);
    globalParameter_->Regist(groupName_, "buttonScale", &buttonScale_);
    globalParameter_->Regist(groupName_, "arrowScale", &arrowScale_);
    globalParameter_->Regist(groupName_, "maxVisibleColumn", &maxVisibleColumn_);
    globalParameter_->Regist(groupName_, "maxVisibleRow", &maxVisibleRow_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void ComboAsistController::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("Base Position", &basePosition_.x, 0.1f);
        ImGui::DragFloat2("Arrow Offset", &arrowOffset_.x, 0.1f);
        ImGui::DragFloat("Column Spacing", &columnSpacing_, 0.1f);
        ImGui::DragFloat("Row Spacing", &rowSpacing_, 0.1f);
        ImGui::DragFloat("Branch Y Offset", &branchYOffset_, 0.1f);
        ImGui::DragFloat("Button Scale", &buttonScale_, 0.01f);
        ImGui::DragFloat("Arrow Scale", &arrowScale_, 0.01f);

        ImGui::Separator();
        ImGui::DragInt("Max Visible Column", &maxVisibleColumn_, 1);
        ImGui::DragInt("Max Visible Row", &maxVisibleRow_, 1);

        // リアルタイム再配置（全条件再構築）
        if (ImGui::Button("Rebuild UI")) {
            BuildAllConditions();
            // 現在の条件以外を非表示
            for (auto& [cond, data] : conditionDataMap_) {
                if (cond != currentCondition_) {
                    SetConditionVisible(data, false);
                }
            }
            ApplySlideOffset();
            UpdateVisibility();
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
