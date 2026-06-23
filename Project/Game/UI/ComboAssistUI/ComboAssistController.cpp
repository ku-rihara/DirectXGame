#include "ComboAssistController.h"
#include "Scene/GameObj.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "Input/InputData.h"
#include "KillCounter/KillCounter.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "Player/Player.h"
#include <algorithm>
#include <imgui.h>

///==========================================================
/// Init
///==========================================================
void ComboAssistController::Init() {
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
    slideInEasing_.Init("ComboAssistSlideIn.json");
    slideInEasing_.SetAdaptValue(&slideOffsetX_);
    slideInEasing_.Reset();

    slideOutEasing_.Init("ComboAssistSlideOut.json");
    slideOutEasing_.SetAdaptValue(&slideOffsetX_);
    slideOutEasing_.Reset();

    activeSlideEasing_   = nullptr;
    isVisible_           = false;
    isSliding_           = false;
    panelMode_           = PanelMode::Close;
    isModeTransitioning_ = false;

    // 全条件を明示的に非表示化
    for (auto& [cond, data] : conditionDataMap_) {
        uiBuilder_.ApplyToCondition(data, [](BaseComboAssistUI& ui) { ui.SetVisible(false); });
    }

    slideOffsetX_ = slideInEasing_.GetStartValue();
    ApplySlideOffset();
}

///==========================================================
/// Update
///==========================================================
void ComboAssistController::Update() {
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
                    uiBuilder_.ApplyToCondition(next, [](BaseComboAssistUI& ui) { ui.SnapToTarget(); });
                    // isInRange_をfalseにリセットしてからUpdateConditionVisibilityでScaleInを発火
                    uiBuilder_.ApplyToCondition(next, [](BaseComboAssistUI& ui) { ui.SnapRangeState(false); });
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
        auto* comboBehavior = pPlayer_ ? pPlayer_->GetBehaviors().GetComboBehavior() : nullptr;
        if (comboBehavior) {
            columnScroller_.CheckRowShift(*currentData, isVisible_, comboBehavior->GetName());
        }

        visibilityController_.UpdateConditionVisibility(*currentData);
        uiBuilder_.ApplyToCondition(*currentData, [](BaseComboAssistUI& ui) { ui.Update(); });

    }
}

///==========================================================
/// UI再構築
///==========================================================
void ComboAssistController::RebuildAllConditions() {
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
void ComboAssistController::StartSlideIn() {
    activeSlideEasing_ = &slideInEasing_;
    activeSlideEasing_->Reset();
    isSliding_ = true;
    isVisible_ = true;
}

void ComboAssistController::StartSlideOut() {
    activeSlideEasing_ = &slideOutEasing_;
    activeSlideEasing_->Reset();
    isSliding_ = true;
}

void ComboAssistController::UpdateSlide(float deltaTime) {
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

void ComboAssistController::ApplySlideOffset() {
    auto* currentData = GetCurrentData();
    if (currentData) {
        ApplySlideOffsetToCondition(*currentData);
    }
}

void ComboAssistController::ApplySlideOffsetToCondition(ConditionUIData& data) {
    uiBuilder_.ApplyToCondition(data, [&](BaseComboAssistUI& ui) {
        ui.ApplySlideOffset(slideOffsetX_);
    });
}

///==========================================================
/// パネル開閉
///==========================================================
void ComboAssistController::OpenPanel() {
    if (panelMode_ == PanelMode::Open) {
        return;
    }
    panelMode_ = PanelMode::Open;
    StartSlideIn();
    isModeTransitioning_ = true;
}

void ComboAssistController::TogglePanelMode() {
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

void ComboAssistController::CheckToggleInput() {
    if (KetaEngine::Input::IsTriggerPad(0, GamepadButton::DPadUp)) {
        TogglePanelMode();
    }
}

///==========================================================
/// 攻撃発動演出
///==========================================================
void ComboAssistController::UpdateComboState() {
    if (!pPlayer_) {
        return;
    }
    auto* currentData = GetCurrentData();
    if (!currentData) {
        return;
    }
    auto* comboBehavior = pPlayer_->GetBehaviors().GetComboBehavior();
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

void ComboAssistController::SetGroupActiveOutLines(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::unordered_set<std::string>& activeAttacks) {

    uiBuilder_.ForEachStepButton(pathGroup, uiGroup,
        [&](const ComboPathBuilder::ComboStep& step, ComboAssistButtonUI& btn) {
            btn.SetActiveOutLine(activeAttacks.count(step.attackName) > 0);
        });
}

void ComboAssistController::PlayPushScalingForAttack(
    const ComboPathBuilder::ComboPathGroup& pathGroup,
    ComboUIGroup& uiGroup,
    const std::string& attackName) {
    uiBuilder_.ForEachStepButton(pathGroup, uiGroup,
        [&](const ComboPathBuilder::ComboStep&, ComboAssistButtonUI& btn) {
            btn.TryPlayPushScaling(attackName);
        });
}

///==========================================================
/// ヘルパー
///==========================================================
ConditionUIData* ComboAssistController::GetCurrentData() {
    auto it = conditionDataMap_.find(currentCondition_);
    return (it != conditionDataMap_.end()) ? &it->second : nullptr;
}

LayoutParam ComboAssistController::MakeLayoutParam() const {
    return {basePosition_, arrowOffset_, columnSpacing_, rowSpacing_, yGroupOffsetY_, buttonScale_, arrowScale_};
}

///==========================================================
/// パラメータ
///==========================================================
void ComboAssistController::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePosition", &basePosition_);
    globalParameter_->Regist(groupName_, "arrowOffset", &arrowOffset_);
    globalParameter_->Regist(groupName_, "columnSpacing", &columnSpacing_);
    globalParameter_->Regist(groupName_, "rowSpacing", &rowSpacing_);
    globalParameter_->Regist(groupName_, "yGroupOffsetY", &yGroupOffsetY_);
    globalParameter_->Regist(groupName_, "buttonScale", &buttonScale_);
    globalParameter_->Regist(groupName_, "arrowScale", &arrowScale_);
    globalParameter_->Regist(groupName_, "maxVisibleColumn", &maxVisibleColumn_);
    globalParameter_->Regist(groupName_, "maxVisibleRow", &maxVisibleRow_);
    globalParameter_->Regist(groupName_, "remainCountOffset",       &remainCountOffset_);
    globalParameter_->Regist(groupName_, "remainCountDigitSpacing", &remainCountDigitSpacing_);
    globalParameter_->Regist(groupName_, "remainCountScale",        &remainCountScale_);
}

void ComboAssistController::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::DragFloat2("Base Position", &basePosition_.x, 0.1f);
        ImGui::DragFloat2("Arrow Offset", &arrowOffset_.x, 0.1f);
        ImGui::DragFloat("Column Spacing", &columnSpacing_, 0.1f);
        ImGui::DragFloat("Row Spacing", &rowSpacing_, 0.1f);
        ImGui::DragFloat("Y Group Offset Y", &yGroupOffsetY_, 0.1f);
        ImGui::DragFloat("Button Scale", &buttonScale_, 0.01f);
        ImGui::DragFloat("Arrow Scale", &arrowScale_, 0.01f);
        ImGui::Separator();
        ImGui::DragInt("Max Visible Column", &maxVisibleColumn_, 1);
        ImGui::DragInt("Max Visible Row", &maxVisibleRow_, 1);

        if (ImGui::Button("Rebuild UI")) {
            RebuildAllConditions();
            for (auto& [cond, data] : conditionDataMap_) {
                uiBuilder_.ApplyToCondition(data, [](BaseComboAssistUI& ui) { ui.SetVisible(false); });
            }
            visibilityController_.SetVisibleRange(maxVisibleColumn_, maxVisibleRow_);
            columnScroller_.SetVisibleRange(maxVisibleColumn_, maxVisibleRow_);
            ApplySlideOffset();
            auto* currentData = GetCurrentData();
            if (currentData) {
                visibilityController_.UpdateConditionVisibility(*currentData);
            }
        }

        ImGui::SeparatorText("残りキル数表示");
        ImGui::DragFloat2("Remain Count Offset",        &remainCountOffset_.x,       0.1f);
        ImGui::DragFloat2("Remain Count Digit Spacing", &remainCountDigitSpacing_.x, 0.1f);
        ImGui::DragFloat2("Remain Count Scale",         &remainCountScale_.x,        0.01f);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);
        ImGui::PopID();
    }
#endif
}

void ComboAssistController::Connect(GameObj* go) {
    SetAttackController(go->playerComboAttackController_.get());
    SetPlayer(go->player_.get());
    SetKillCounter(go->killCounter_.get());
    Init();
}