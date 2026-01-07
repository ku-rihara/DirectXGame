#include "DeathTimer.h"
#include "Frame/Frame.h"
#include <imgui.h>

void DeathTimer::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    deathTimerGauge_ = std::make_unique<DeathTimerGauge>();
    deathTimerGauge_->Init();
}

void DeathTimer::Update(float timer) {
    currentTimer_ -= timer;
    deathTimerGauge_->SetTimer(currentTimer_, maxTimer_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void DeathTimer::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("decrementSpeedRate", &decrementSpeedRate_, 0.01f);
        ImGui::DragFloat("Increment Time", &incrementTime_, 0.01f);
        ImGui::DragFloat("increment By ComboLevelUP", &incrementByComboLevelUP_, 0.01f);
        ImGui::DragFloat("maxTimer", &maxTimer_, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif // _DEBUG
}

void DeathTimer::RegisterParams() {
    globalParameter_->Regist(groupName_, "decrementSpeedRate", &decrementSpeedRate_);
    globalParameter_->Regist(groupName_, "incrementTime", &incrementTime_);
    globalParameter_->Regist(groupName_, "incrementByComboLevelUP", &incrementByComboLevelUP_);
    globalParameter_->Regist(groupName_, "maxTimer", &maxTimer_);
}
