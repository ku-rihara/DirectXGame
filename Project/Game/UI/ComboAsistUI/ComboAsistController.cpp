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
/// Init
///==========================================================
void ComboAsistController::Init() {
    if (!pAttackController_) {
        return;
    }

    // グローバルパラメータの初期化
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // コンディションの初期状態
    currentCondition_ = PlayerComboAttackData::TriggerCondition::GROUND;
    RebuildAllConditions();

    // 分割クラスの初期化
    visibilityController_.SetVisibleRange(maxVisibleColumn_, maxVisibleRow_);

    columnScroller_.SetVisibleRange(maxVisibleColumn_, maxVisibleRow_);
    columnScroller_.Init(&visibilityController_);

    conditionSwitcher_.Init(
        pAttackController_,
        &uiBuilder_,
        &conditionDataMap_,
        &currentCondition_);

    // スライドイージングの初期化
    slideInEasing_.Init("ComboAsistSlideIn.json");
    slideInEasing_.SetAdaptValue(&slideOffsetX_);
    slideInEasing_.Reset();

    slideOutEasing_.Init("ComboAsistSlideOut.json");
    slideOutEasing_.SetAdaptValue(&slideOffsetX_);
    slideOutEasing_.Reset();

    activeSlideEasing_   = nullptr;
    isVisible_           = false;
    isSliding_           = false;
    panelMode_           = PanelMode::Close;
    isModeTransitioning_ = false;

    // 全条件を明示的に非表示化
    for (auto& [cond, data] : conditionDataMap_) {
        uiBuilder_.ApplyToCondition(data, [](BaseComboAsistUI& ui) { ui.SetVisible(false); });
    }

    slideOffsetX_ = slideInEasing_.GetStartValue();
    ApplySlideOffset();
}

///==========================================================
/// Update
///==========================================================
void ComboAsistController::Update() {
    conditionSwitcher_.SyncUnlockStates();
    UpdateComboState();
    CheckToggleInput();

// 自動条件切替（プレイヤー状態：空中/ダッシュ/地上）
    {
        PlayerComboAttackData::TriggerCondition autoTarget;
        if (conditionSwitcher_.CheckAutoConditionSwitch(isVisible_, pPlayer_, autoTarget)) {
            conditionSwitcher_.SwitchCondition(autoTarget,
                [&](ConditionUIData&, ConditionUIData& next) {
                    ApplySlideOffset();
                    uiBuilder_.ApplyToCondition(next, [](BaseComboAsistUI& ui) { ui.SnapToTarget(); });
                    visibilityController_.UpdateConditionVisibility(next);
                    playedAttacks_.clear();
                });
        }
    }

    UpdateSlide(KetaEngine::Frame::DeltaTime());
    ApplySlideOffset();

    auto* currentData = GetCurrentData();
    if (currentData) {
        columnScroller_.CheckColumnOverflow(*currentData, isVisible_);

        // 行シフト
        auto* comboBehavior = pPlayer_ ? pPlayer_->GetComboBehavior() : nullptr;
        if (comboBehavior) {
            columnScroller_.CheckRowShift(*currentData, isVisible_, comboBehavior->GetName());
        }

        visibilityController_.UpdateConditionVisibility(*currentData);
        uiBuilder_.ApplyToCondition(*currentData, [](BaseComboAsistUI& ui) { ui.Update(); });
    }
}

///==========================================================
/// UI再構築
///==========================================================
void ComboAsistController::RebuildAllConditions() {
    // 全攻撃発動条件のコンボアシストUIを構築
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
        isSliding_           = false;
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

void ComboAsistController::ApplySlideOffsetToCondition(ConditionUIData& data) {
    uiBuilder_.ApplyToCondition(data, [&](BaseComboAsistUI& ui) {
        ui.ApplySlideOffset(slideOffsetX_);
    });
}

///==========================================================
/// パネル開閉
///==========================================================
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
    if (panelMode_ == PanelMode::Open) {
        panelMode_ = PanelMode::Close;
        StartSlideOut();
    } else {
        panelMode_ = PanelMode::Open;
        StartSlideIn();
    }
    isModeTransitioning_ = true;
}

void ComboAsistController::CheckToggleInput() {
    if (KetaEngine::Input::IsTriggerPad(0, GamepadButton::DPadUp)) {
        TogglePanelMode();
    }
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
        // コンボリセット：アウトライン・シフト・条件をすべて戻す
        if (!playedAttacks_.empty()) {
            playedAttacks_.clear();
            SetGroupActiveOutLines(currentData->pathBuilder.GetXGroup(), currentData->xUIGroup, playedAttacks_);
            SetGroupActiveOutLines(currentData->pathBuilder.GetYGroup(), currentData->yUIGroup, playedAttacks_);
        }

        columnScroller_.ResetShifts(*currentData);
        visibilityController_.SnapConditionVisibility(*currentData);
    } else {
        // 攻撃発動中
        bool isNew = (name != prevBehaviorName_);
        playedAttacks_.insert(name);
        SetGroupActiveOutLines(currentData->pathBuilder.GetXGroup(), currentData->xUIGroup, playedAttacks_);
        SetGroupActiveOutLines(currentData->pathBuilder.GetYGroup(), currentData->yUIGroup, playedAttacks_);

        if (isNew) {
            PlayPushScalingForAttack(currentData->pathBuilder.GetXGroup(), currentData->xUIGroup, name);
            PlayPushScalingForAttack(currentData->pathBuilder.GetYGroup(), currentData->yUIGroup, name);

            columnScroller_.ShiftColumnsForAttack(
                *currentData,
                name,
                maxVisibleColumn_,
                currentData->pathBuilder.GetXGroup(),
                currentData->pathBuilder.GetYGroup());
        }
    }

    prevBehaviorName_ = name;
}

void ComboAsistController::SetGroupActiveOutLines(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::unordered_set<std::string>& activeAttacks) {

    uiBuilder_.ForEachStepButton(pathGroup, uiGroup,
        [&](const ComboPathBuilder::ComboStep& step, ComboAsistButtonUI& btn) {
            btn.SetActiveOutLine(activeAttacks.count(step.attackName) > 0);
        });
}

void ComboAsistController::PlayPushScalingForAttack(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::string& attackName) {
    uiBuilder_.ForEachStepButton(pathGroup, uiGroup,
        [&](const ComboPathBuilder::ComboStep&, ComboAsistButtonUI& btn) {
            btn.TryPlayPushScaling(attackName);
        });
}

///==========================================================
/// ヘルパー
///==========================================================
ConditionUIData* ComboAsistController::GetCurrentData() {
    auto it = conditionDataMap_.find(currentCondition_);
    return (it != conditionDataMap_.end()) ? &it->second : nullptr;
}

LayoutParam ComboAsistController::MakeLayoutParam() const {
    return {basePosition_, arrowOffset_, columnSpacing_, rowSpacing_, yGroupOffsetY_, buttonScale_, arrowScale_};
}

///==========================================================
/// パラメータ
///==========================================================
void ComboAsistController::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePosition",     &basePosition_);
    globalParameter_->Regist(groupName_, "arrowOffset",      &arrowOffset_);
    globalParameter_->Regist(groupName_, "columnSpacing",    &columnSpacing_);
    globalParameter_->Regist(groupName_, "rowSpacing",       &rowSpacing_);
    globalParameter_->Regist(groupName_, "yGroupOffsetY",    &yGroupOffsetY_);
    globalParameter_->Regist(groupName_, "buttonScale",      &buttonScale_);
    globalParameter_->Regist(groupName_, "arrowScale",       &arrowScale_);
    globalParameter_->Regist(groupName_, "maxVisibleColumn", &maxVisibleColumn_);
    globalParameter_->Regist(groupName_, "maxVisibleRow",    &maxVisibleRow_);
}

void ComboAsistController::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::DragFloat2("Base Position",    &basePosition_.x, 0.1f);
        ImGui::DragFloat2("Arrow Offset",     &arrowOffset_.x,  0.1f);
        ImGui::DragFloat("Column Spacing",    &columnSpacing_,  0.1f);
        ImGui::DragFloat("Row Spacing",       &rowSpacing_,     0.1f);
        ImGui::DragFloat("Y Group Offset Y",   &yGroupOffsetY_,  0.1f);
        ImGui::DragFloat("Button Scale",      &buttonScale_,    0.01f);
        ImGui::DragFloat("Arrow Scale",       &arrowScale_,     0.01f);
        ImGui::Separator();
        ImGui::DragInt("Max Visible Column",  &maxVisibleColumn_, 1);
        ImGui::DragInt("Max Visible Row",     &maxVisibleRow_,    1);

        if (ImGui::Button("Rebuild UI")) {
            RebuildAllConditions();
            for (auto& [cond, data] : conditionDataMap_) {
                uiBuilder_.ApplyToCondition(data, [](BaseComboAsistUI& ui) { ui.SetVisible(false); });
            }
            visibilityController_.SetVisibleRange(maxVisibleColumn_, maxVisibleRow_);
            columnScroller_.SetVisibleRange(maxVisibleColumn_, maxVisibleRow_);
            ApplySlideOffset();
            auto* currentData = GetCurrentData();
            if (currentData) {
                visibilityController_.UpdateConditionVisibility(*currentData);
            }
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
#endif
}
