#include "ComboAsistController.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "input/InputData.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <imgui.h>


void ComboAsistController::Init() {
    if (!pAttackController_) {
        return;
    }

    // globalParameterのセット
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // 全ての攻撃Conditionの攻撃を構築
    currentCondition_ = PlayerComboAttackData::TriggerCondition::GROUND;
    RebuildAllConditions();


    slideInEasing_.Init("ComboAsistSlideIn.json");
    slideInEasing_.SetAdaptValue(&slideOffsetX_);
    slideInEasing_.Reset();

    slideOutEasing_.Init("ComboAsistSlideOut.json");
    slideOutEasing_.SetAdaptValue(&slideOffsetX_);
    slideOutEasing_.Reset();

    activeSlideEasing_  = nullptr;
    isVisible_          = false;
    isSliding_          = false;
    panelMode_          = PanelMode::Close;
    isModeTransitioning_ = false;

    columnOverflowEasing_.Init("ComboAsistColumnOverflow.json");
    columnOverflowEasing_.SetAdaptValue(&overflowScale_);
    columnOverflowEasing_.Reset();
    isAutoSwitchedCondition_ = false;

    // スプライトはデフォルトで表示状態のため、全条件を明示的に非表示化
    // 実際の表示はパネルオープン後にUpdateVisibilityが行う
    for (auto& [cond, data] : conditionDataMap_) {
        SetConditionVisible(data, false);
    }

    slideOffsetX_ = slideInEasing_.GetStartValue();
    ApplySlideOffset();
}

///==========================================================
/// Update
///==========================================================
void ComboAsistController::Update() {
    SyncUnlockStates();
    UpdateComboState();
    CheckToggleInput();
    CheckConditionSwitchInput();
    CheckAutoConditionSwitch();
    UpdateSlide(KetaEngine::Frame::DeltaTime());
    ApplySlideOffset();
    CheckColumnOverflow();
    CheckRowShift();
    UpdateVisibility();

    UpdateCurrentConditionUI();
}


void ComboAsistController::RebuildAllConditions() {
    // 全攻撃ConditionのUIを構築
    uiBuilder_.BuildAllConditions(
        pAttackController_,
        MakeLayoutParam(),
        conditionDataMap_,
        availableConditions_,
        currentCondition_);
}

///==========================================================
/// スライドイン・アウト
///==========================================================
void ComboAsistController::StartSlideIn() {
    activeSlideEasing_ = &slideInEasing_;
    activeSlideEasing_->Reset();
    isSliding_ = true;
    isVisible_ = true;
}

void ComboAsistController::StartSlideOut() {
    activeSlideEasing_ = &slideOutEasing_;
    activeSlideEasing_->Reset();
    isSliding_ = true;
}

void ComboAsistController::UpdateSlide(float deltaTime) {
    if (!isSliding_ || !activeSlideEasing_) {
        return;
    }
    // アクティブなイージングを更新
    activeSlideEasing_->Update(deltaTime);

    // アシストUIのCLOSEまたはOPENの完了
    if (activeSlideEasing_->IsFinished()) {
        isSliding_ = false;
        isModeTransitioning_ = false;
        if (activeSlideEasing_ == &slideOutEasing_) {
            isVisible_ = false;
        }
    }
}

void ComboAsistController::ApplySlideOffset() {
    auto* currentData = GetCurrentData();
    if (currentData) {
        ApplySlideOffsetToCondition(*currentData);
    }
}

void ComboAsistController::OpenPanel() {
    if (panelMode_ == PanelMode::Open) {
        return;
    }
    panelMode_ = PanelMode::Open;
    StartSlideIn();
    isModeTransitioning_ = true;
}

void ComboAsistController::TogglePanelMode() {
    if (isModeTransitioning_) {
        return;
    }

    // モード切替
    if (panelMode_ == PanelMode::Open) {
        panelMode_ = PanelMode::Close;
        StartSlideOut();
        isModeTransitioning_ = true;
    } else {
        panelMode_ = PanelMode::Open;
        StartSlideIn();
        isModeTransitioning_ = true;
    }
}

void ComboAsistController::CheckToggleInput() {
    if (KetaEngine::Input::IsTriggerPad(0, GamepadButton::DPadUp)) {
        TogglePanelMode();
    }
}

///==========================================================
/// ロック状態の同期
///==========================================================
void ComboAsistController::SyncUnlockStates() {
    if (!pAttackController_) {
        return;
    }
    for (auto& [cond, data] : conditionDataMap_) {
        SyncGroupUnlockStates(data.pathBuilder.GetXGroup(), data.xUIGroup);
        SyncGroupUnlockStates(data.pathBuilder.GetYGroup(), data.yUIGroup);
    }
}

void ComboAsistController::SyncGroupUnlockStates(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup) {
    uiBuilder_.ForEachStepButton(pathGroup, uiGroup, [&](const ComboPathBuilder::ComboStep& step, ComboAsistButtonUI& btn) {
        auto* attackData = pAttackController_->GetAttackByName(step.attackName);
        if (attackData) {
            btn.SetUnlocked(attackData->GetAttackParam().isUnlocked);
        }
    });
}

///==========================================================
/// 攻撃発動演出
///==========================================================
void ComboAsistController::UpdateComboState() {
    if (!pPlayer_) {
        return;
    }
    auto* currentData = GetCurrentData();
    if (!currentData) {
        return;
    }

    auto* comboBehavior = pPlayer_->GetComboBehavior();
    if (!comboBehavior) {
        return;
    }

    const std::string& name = comboBehavior->GetName();

    if (name == "ComboAttackRoot") {
        if (!playedAttacks_.empty()) {
            playedAttacks_.clear();
            SetGroupActiveOutLines(currentData->pathBuilder.GetXGroup(), currentData->xUIGroup, playedAttacks_);
            SetGroupActiveOutLines(currentData->pathBuilder.GetYGroup(), currentData->yUIGroup, playedAttacks_);
        }
        // Rowリセットを先に行う（divColはカラムシフト後の座標で記録されているため、
        // カラムを先に戻すと座標系がずれてしまう）
        if (currentData->rowShiftAmount != 0) {
            int32_t branchRow = currentData->rowShiftAmount;
            int32_t divCol    = currentData->rowShiftDivergeCol;
            int32_t mainRow   = currentData->rowShiftMainRow;
            int32_t delta     = branchRow - mainRow; // 正方向（復元用）
            // シフトしたグループのみに適用（X/Y独立row座標系のため）
            ComboUIGroup& shiftedGroup = currentData->rowShiftIsXGroup ? currentData->xUIGroup : currentData->yUIGroup;
            // Step1: 現在mainRowにいるbranch itemsをbranchRowへ戻す
            ShiftGroupRows(shiftedGroup, mainRow, divCol, delta);
            // Step2: 範囲外にいるmain items（row = mainRow-delta）をmainRowへ戻す
            ShiftGroupRows(shiftedGroup, mainRow - delta, divCol, delta);
            currentData->rowShiftAmount     = 0;
            currentData->rowShiftDivergeCol = -1;
            currentData->rowShiftMainRow    = 0;
        }
        // Columnリセット（rowリセット後に行うことで座標系が一致する）
        if (currentData->xUIGroup.columnShiftAmount != 0) {
            ShiftGroupColumns(currentData->xUIGroup, currentData->xUIGroup.columnShiftAmount);
        }
        if (currentData->yUIGroup.columnShiftAmount != 0) {
            ShiftGroupColumns(currentData->yUIGroup, currentData->yUIGroup.columnShiftAmount);
        }
        if (isAutoSwitchedCondition_) {
            isAutoSwitchedCondition_ = false;
            SwitchCondition(PlayerComboAttackData::TriggerCondition::GROUND);
        }
    } else {
        bool isNew = (name != prevBehaviorName_);
        playedAttacks_.insert(name);
        SetGroupActiveOutLines(currentData->pathBuilder.GetXGroup(), currentData->xUIGroup, playedAttacks_);
        SetGroupActiveOutLines(currentData->pathBuilder.GetYGroup(), currentData->yUIGroup, playedAttacks_);
        if (isNew) {
            PlayPushScalingForAttack(currentData->pathBuilder.GetXGroup(), currentData->xUIGroup, name);
            PlayPushScalingForAttack(currentData->pathBuilder.GetYGroup(), currentData->yUIGroup, name);
            // 発動攻撃が表示範囲端に達したらグループ別にシフト
            int32_t xActiveCol = FindAttackColumnInGroup(currentData->xUIGroup, name);
            if (xActiveCol >= maxVisibleColumn_ && !currentData->xUIGroup.isColumnShifting) {
                int32_t shiftAmount = xActiveCol - maxVisibleColumn_ + 1;
                for (int32_t c = 0; c < shiftAmount; ++c) {
                    TriggerLeaveRangeForColumn(currentData->xUIGroup, c);
                }
                ShiftGroupColumns(currentData->xUIGroup, -shiftAmount);
            }
            int32_t yActiveCol = FindAttackColumnInGroup(currentData->yUIGroup, name);
            if (yActiveCol >= maxVisibleColumn_ && !currentData->yUIGroup.isColumnShifting) {
                int32_t shiftAmount = yActiveCol - maxVisibleColumn_ + 1;
                for (int32_t c = 0; c < shiftAmount; ++c) {
                    TriggerLeaveRangeForColumn(currentData->yUIGroup, c);
                }
                ShiftGroupColumns(currentData->yUIGroup, -shiftAmount);
            }
        }
    }
    prevBehaviorName_ = name;
}

void ComboAsistController::SetGroupActiveOutLines(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::unordered_set<std::string>& activeAttacks) {
    uiBuilder_.ForEachStepButton(pathGroup, uiGroup, [&](const ComboPathBuilder::ComboStep& step, ComboAsistButtonUI& btn) {
        btn.SetActiveOutLine(activeAttacks.count(step.attackName) > 0);
    });
}

void ComboAsistController::PlayPushScalingForAttack(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::string& attackName) {
    uiBuilder_.ForEachStepButton(pathGroup, uiGroup, [&](const ComboPathBuilder::ComboStep&, ComboAsistButtonUI& btn) {
        btn.TryPlayPushScaling(attackName);
    });
}

///==========================================================
/// 発動条件切替
///==========================================================
void ComboAsistController::CheckConditionSwitchInput() {
    if (panelMode_ != PanelMode::Open) {
        return;
    }
    if (!KetaEngine::Input::IsTriggerPad(0, GamepadButton::DPadRight)) {
        return;
    }
    if (availableConditions_.size() <= 1) {
        return;
    }

    auto found               = std::find(availableConditions_.begin(), availableConditions_.end(), currentCondition_);
    size_t idx = 0;
    if (found != availableConditions_.end()) {
        idx = static_cast<size_t>(found - availableConditions_.begin());
    }
    isAutoSwitchedCondition_ = false;
    SwitchCondition(availableConditions_[(idx + 1) % availableConditions_.size()]);
}

void ComboAsistController::CheckAutoConditionSwitch() {
    using TC = PlayerComboAttackData::TriggerCondition;
    if (!isVisible_ || !pPlayer_ || !pAttackController_ || currentCondition_ != TC::GROUND) {
        return;
    }

    auto* comboBehavior = pPlayer_->GetComboBehavior();
    if (!comboBehavior) {
        return;
    }
    const std::string& name = comboBehavior->GetName();
    if (name == "ComboAttackRoot" || name == prevBehaviorName_) {
        return;
    }

    auto* attackData = pAttackController_->GetAttackByName(name);
    if (attackData && attackData->GetAttackParam().triggerParam.condition == TC::AIR && conditionDataMap_.count(TC::AIR) > 0) {
        isAutoSwitchedCondition_ = true;
        SwitchCondition(TC::AIR);
    }
}

void ComboAsistController::SwitchCondition(PlayerComboAttackData::TriggerCondition condition) {
    if (conditionDataMap_.count(condition) == 0 || condition == currentCondition_) {
        return;
    }
    if (conditionDataMap_.count(currentCondition_) > 0) {
        SetConditionVisible(conditionDataMap_[currentCondition_], false);
    }
    currentCondition_ = condition;
    ApplySlideOffset();
    SnapConditionToTarget(conditionDataMap_[currentCondition_]);
    UpdateVisibility();
    playedAttacks_.clear();
}

void ComboAsistController::SetConditionVisible(ConditionUIData& data, bool visible) {
    uiBuilder_.ApplyToCondition(data, [visible](BaseComboAsistUI& ui) { ui.SetVisible(visible); });
}

///==========================================================
/// 表示範囲・列オーバーフロー
///==========================================================
void ComboAsistController::CheckColumnOverflow() {
    if (!isVisible_) {
        return;
    }
    auto* currentData = GetCurrentData();
    if (!currentData) {
        return;
    }

    // X/Yグループのオーバーフロー処理（イージングは共有、更新は1回だけ）
    bool anyShifting = currentData->xUIGroup.isColumnShifting || currentData->yUIGroup.isColumnShifting;
    if (anyShifting) {
        columnOverflowEasing_.Update(KetaEngine::Frame::DeltaTime());
        if (columnOverflowEasing_.IsFinished()) {
            if (currentData->xUIGroup.isColumnShifting) {
                currentData->xUIGroup.isColumnShifting = false;
                ShiftGroupColumns(currentData->xUIGroup, -1);
            }
            if (currentData->yUIGroup.isColumnShifting) {
                currentData->yUIGroup.isColumnShifting = false;
                ShiftGroupColumns(currentData->yUIGroup, -1);
            }
        }
        return;
    }

    CheckGroupColumnOverflowDetect(currentData->xUIGroup);
    CheckGroupColumnOverflowDetect(currentData->yUIGroup);
}

void ComboAsistController::ShiftGroupColumns(ComboUIGroup& uiGroup, int32_t delta) {
    ShiftGroup(uiGroup, delta);
    uiGroup.columnShiftAmount -= delta;
}

void ComboAsistController::UpdateVisibility() {
    if (!isVisible_) {
        return;
    }
    auto* currentData = GetCurrentData();
    if (!currentData) {
        return;
    }

    UpdateGroupVisibility(currentData->xUIGroup);
    UpdateGroupVisibility(currentData->yUIGroup);
}

///==========================================================
/// ラムダ置き換え用メンバ関数
///==========================================================
void ComboAsistController::UpdateCurrentConditionUI() {
    auto* currentData = GetCurrentData();
    if (currentData) {
        uiBuilder_.ApplyToCondition(*currentData, [](BaseComboAsistUI& ui) { ui.Update(); });
    }
}

void ComboAsistController::ApplySlideOffsetToCondition(ConditionUIData& data) {
    uiBuilder_.ApplyToCondition(data, [&](BaseComboAsistUI& ui) { ui.ApplySlideOffset(slideOffsetX_); });
}

void ComboAsistController::SnapConditionToTarget(ConditionUIData& data) {
    uiBuilder_.ApplyToCondition(data, [](BaseComboAsistUI& ui) { ui.SnapToTarget(); });
}

void ComboAsistController::CheckGroupColumnOverflowDetect(ComboUIGroup& uiGroup) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetState() != BaseComboAsistUI::AsistState::NONE && btn->GetColumnNum() > maxVisibleColumn_) {
            uiGroup.isColumnShifting = true;
            columnOverflowEasing_.Reset();
            columnOverflowEasing_.SetAdaptValue(&overflowScale_);
            TriggerLeaveRangeForColumn(uiGroup, 0);
            return;
        }
    }
}

void ComboAsistController::ShiftGroup(ComboUIGroup& uiGroup, int32_t delta) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        btn->SetRowColumn(btn->GetRowNum(), btn->GetColumnNum() + delta);
        btn->ApplyLayout();
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            btn->SetRowColumn(btn->GetRowNum(), btn->GetColumnNum() + delta);
            btn->ApplyLayout();
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        arrow->ShiftColumns(delta);
        arrow->ApplyLayout();
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        arrow->ShiftColumns(delta);
        arrow->ApplyLayout();
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            arrow->ShiftColumns(delta);
            arrow->ApplyLayout();
        }
    }
}

bool ComboAsistController::IsInVisibleRange(int32_t col, int32_t row) const {
    return col >= 0 && col <= maxVisibleColumn_ && row >= 0 && row <= maxVisibleRow_;
}

bool ComboAsistController::IsArrowVisible(const ComboAsistArrowUI& arrow) const {
    return arrow.GetFromCol() >= 0 && arrow.GetToCol() <= maxVisibleColumn_ &&
           arrow.GetFromRow() >= 0 && arrow.GetToRow() <= maxVisibleRow_;
}

void ComboAsistController::UpdateGroupVisibility(ComboUIGroup& uiGroup) {
    ApplyRangeVisibleToGroup(uiGroup);
}

void ComboAsistController::ApplyRangeVisibleToGroup(ComboUIGroup& uiGroup) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        btn->SetRangeVisible(IsInVisibleRange(btn->GetColumnNum(), btn->GetRowNum()));
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            btn->SetRangeVisible(IsInVisibleRange(btn->GetColumnNum(), btn->GetRowNum()));
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        arrow->SetRangeVisible(IsArrowVisible(*arrow));
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        arrow->SetRangeVisible(IsArrowVisible(*arrow));
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            arrow->SetRangeVisible(IsArrowVisible(*arrow));
        }
    }
}

void ComboAsistController::TriggerLeaveRangeForColumn(ComboUIGroup& uiGroup, int32_t col) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetColumnNum() == col) {
            btn->SetRangeVisible(false);
        }
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            if (btn->GetColumnNum() == col) {
                btn->SetRangeVisible(false);
            }
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        if (arrow->GetFromCol() == col) {
            arrow->SetRangeVisible(false);
        }
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        if (arrow->GetFromCol() == col) {
            arrow->SetRangeVisible(false);
        }
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            if (arrow->GetFromCol() == col) {
                arrow->SetRangeVisible(false);
            }
        }
    }
}

///==========================================================
/// Row shift
///==========================================================
void ComboAsistController::CheckRowShift() {
    if (!isVisible_ || !pPlayer_) {
        return;
    }
    auto* currentData = GetCurrentData();
    if (!currentData || currentData->rowShiftAmount != 0) {
        return;
    }

    auto* comboBehavior = pPlayer_->GetComboBehavior();
    if (!comboBehavior) {
        return;
    }
    const std::string& name = comboBehavior->GetName();
    if (name == "ComboAttackRoot") {
        return;
    }

    // どちらのグループの分岐にいるか探す
    ComboUIGroup* affectedGroup = nullptr;
    int32_t branchRow           = FindBranchRowForAttack(currentData->xUIGroup, name);
    if (branchRow >= 0) {
        affectedGroup = &currentData->xUIGroup;
        currentData->rowShiftIsXGroup = true;
    } else {
        branchRow = FindBranchRowForAttack(currentData->yUIGroup, name);
        if (branchRow >= 0) {
            affectedGroup = &currentData->yUIGroup;
            currentData->rowShiftIsXGroup = false;
        }
    }

    if (!affectedGroup || branchRow <= 0) {
        return;
    }

    int32_t divCol  = FindDivergeColForBranchRow(*affectedGroup, branchRow);
    if (divCol < 0) {
        return;
    }

    // このグループのmainRowを取得（col < divCol にいるmainButtonの行）
    int32_t mainRow = 0;
    for (auto& btn : affectedGroup->mainButtonUIs) {
        if (btn->GetColumnNum() < divCol) {
            mainRow = btn->GetRowNum();
            break;
        }
    }

    const int32_t delta = mainRow - branchRow; // 負の値

    // Step1: 競合するmain items（mainRow, col>=divCol）を範囲外へ
    ShiftGroupRows(*affectedGroup, mainRow, divCol, delta);
    // Step2: branch items（branchRow, col>=divCol）をmainRowへ
    ShiftGroupRows(*affectedGroup, branchRow, divCol, delta);

    currentData->rowShiftAmount     = branchRow;
    currentData->rowShiftDivergeCol = divCol;
    currentData->rowShiftMainRow    = mainRow;
}

void ComboAsistController::ShiftGroupRows(ComboUIGroup& uiGroup, int32_t fromRow, int32_t divergeCol, int32_t delta) {
    // メインパスのボタン
    for (auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetRowNum() == fromRow && btn->GetColumnNum() >= divergeCol) {
            btn->SetRowColumn(btn->GetRowNum() + delta, btn->GetColumnNum());
            btn->ApplyLayout();
        }
    }
    // 分岐パスのボタン
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            if (btn->GetRowNum() == fromRow && btn->GetColumnNum() >= divergeCol) {
                btn->SetRowColumn(btn->GetRowNum() + delta, btn->GetColumnNum());
                btn->ApplyLayout();
            }
        }
    }
    // メインパスの矢印（始点が fromRow かつ divCol 以降）
    for (auto& arrow : uiGroup.mainArrowUIs) {
        if (arrow->GetFromRow() == fromRow && arrow->GetFromCol() >= divergeCol) {
            arrow->ShiftRows(delta);
            arrow->ApplyLayout();
        }
    }
    // 分岐斜め矢印（終点が fromRow のもの）
    for (auto& arrow : uiGroup.branchArrowUIs) {
        if (arrow->GetToRow() == fromRow) {
            arrow->ShiftRows(delta);
            arrow->ApplyLayout();
        }
    }
    // 分岐内矢印（始点が fromRow のもの）
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            if (arrow->GetFromRow() == fromRow) {
                arrow->ShiftRows(delta);
                arrow->ApplyLayout();
            }
        }
    }
}

int32_t ComboAsistController::FindBranchRowForAttack(const ComboUIGroup& uiGroup, const std::string& attackName) const {
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            if (btn->GetAttackName() == attackName) {
                return btn->GetRowNum();
            }
        }
    }
    return -1;
}

int32_t ComboAsistController::FindDivergeColForBranchRow(const ComboUIGroup& uiGroup, int32_t branchRow) const {
    int32_t minCol = INT32_MAX;
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            if (btn->GetRowNum() == branchRow) {
                if (btn->GetColumnNum() < minCol) {
                    minCol = btn->GetColumnNum();
                }
            }
        }
    }
    return (minCol == INT32_MAX) ? -1 : minCol;
}

///==========================================================
/// ヘルパーメソッド
///==========================================================
ConditionUIData* ComboAsistController::GetCurrentData() {
    auto it = conditionDataMap_.find(currentCondition_);
    if (it != conditionDataMap_.end()) {
        return &it->second;
    }
    return nullptr;
}

int32_t ComboAsistController::FindAttackColumnInGroup(const ComboUIGroup& uiGroup, const std::string& attackName) const {
    for (const auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetAttackName() == attackName) return btn->GetColumnNum();
    }
    for (const auto& buttonRow : uiGroup.branchButtonUIs) {
        for (const auto& btn : buttonRow) {
            if (btn->GetAttackName() == attackName) return btn->GetColumnNum();
        }
    }
    return -1;
}

LayoutParam ComboAsistController::MakeLayoutParam() const {
    return {basePosition_, arrowOffset_, columnSpacing_, rowSpacing_, branchYOffset_, buttonScale_, arrowScale_};
}



///==========================================================
/// パラメータ
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

        if (ImGui::Button("Rebuild UI")) {
            RebuildAllConditions();
            for (auto& [cond, data] : conditionDataMap_) {
                SetConditionVisible(data, false);
            }
            ApplySlideOffset();
            UpdateVisibility();
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
#endif
}
