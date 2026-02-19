#include "ComboAsistController.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "input/InputData.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"
#include <algorithm>
#include <cmath>
#include <imgui.h>

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

LayoutParam ComboAsistController::MakeLayoutParam() const {
    return {basePosition_, arrowOffset_, columnSpacing_, rowSpacing_, branchYOffset_, buttonScale_, arrowScale_};
}


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

    for (auto& [cond, data] : conditionDataMap_) {
        if (cond != currentCondition_) {
            SetConditionVisible(data, false);
        }
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
    activeSlideEasing_->Update(deltaTime);
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
    if (panelMode_ == PanelMode::Open) return;
    panelMode_ = PanelMode::Open;
    StartSlideIn();
    isModeTransitioning_ = true;
}

void ComboAsistController::TogglePanelMode() {
    if (isModeTransitioning_) return;
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
        if (currentData->columnShiftAmount != 0) {
            ShiftAllColumns(currentData->columnShiftAmount);
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
    bool found = false;
    uiBuilder_.ForEachStepButton(pathGroup, uiGroup, [&](const ComboPathBuilder::ComboStep& step, ComboAsistButtonUI& btn) {
        if (!found && step.attackName == attackName) {
            btn.PlayPushScaling();
            found = true;
        }
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

    if (currentData->isColumnShifting) {
        columnOverflowEasing_.Update(KetaEngine::Frame::DeltaTime());
        if (columnOverflowEasing_.IsFinished()) {
            currentData->isColumnShifting = false;
            ShiftAllColumns(-1);
        }
        return;
    }

    CheckGroupColumnOverflow(*currentData, currentData->xUIGroup) || CheckGroupColumnOverflow(*currentData, currentData->yUIGroup);
}

void ComboAsistController::ShiftAllColumns(int32_t delta) {
    auto* currentData = GetCurrentData();
    if (!currentData) {
        return;
    }

    ShiftGroup(currentData->xUIGroup, delta);
    ShiftGroup(currentData->yUIGroup, delta);
    currentData->columnShiftAmount -= delta;
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

bool ComboAsistController::CheckGroupColumnOverflow(ConditionUIData& data, ComboUIGroup& uiGroup) {
    for (auto& btn : uiGroup.mainButtonUIs) {
        if (btn->GetState() != BaseComboAsistUI::AsistState::NONE && btn->GetColumnNum() > maxVisibleColumn_) {
            data.isColumnShifting = true;
            columnOverflowEasing_.Reset();
            for (auto& button : uiGroup.mainButtonUIs) {
                if (button->GetColumnNum() == 0) {
                    columnOverflowEasing_.SetAdaptValue(&overflowScale_);
                    break;
                }
            }
            return true;
        }
    }
    return false;
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
    for (auto& btn : uiGroup.mainButtonUIs) {
        btn->SetVisible(IsInVisibleRange(btn->GetColumnNum(), btn->GetRowNum()));
    }
    for (auto& buttonRow : uiGroup.branchButtonUIs) {
        for (auto& btn : buttonRow) {
            btn->SetVisible(IsInVisibleRange(btn->GetColumnNum(), btn->GetRowNum()));
        }
    }
    for (auto& arrow : uiGroup.mainArrowUIs) {
        arrow->SetVisible(IsInVisibleRange(arrow->GetColumnNum(), arrow->GetRowNum()));
    }
    for (auto& arrow : uiGroup.branchArrowUIs) {
        arrow->SetVisible(IsArrowVisible(*arrow));
    }
    for (auto& arrowRow : uiGroup.branchInnerArrowUIs) {
        for (auto& arrow : arrowRow) {
            arrow->SetVisible(IsArrowVisible(*arrow));
        }
    }
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
                if (cond != currentCondition_) {
                    SetConditionVisible(data, false);
                }
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
