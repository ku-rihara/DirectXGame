#include "StressGauge.h"
#include "Frame/Frame.h"
#include <algorithm>
#include <imgui.h>

void StressGauge::Init() {

    // globalParameter初期化
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
    
    // StressGaugeGauge初期化
    StressGaugeGauge_ = std::make_unique<StressGaugeGauge>();
    StressGaugeGauge_->Init();
    StressGaugeGauge_->SetTimer(0.0f, maxStress_);

    currentStress_ = 0.0f;
    isDeath_       = false;
}

void StressGauge::Update(float deltaTime) {

    (void)deltaTime;

    currentStress_ = std::clamp(currentStress_, 0.0f, maxStress_);

    if (currentStress_ >= maxStress_) {
        isDeath_ = true;
    }

    // SetTimer で目標値を先に更新し、Update でイージング処理する
    StressGaugeGauge_->SetTimer(currentStress_, maxStress_);
    StressGaugeGauge_->Update(deltaTime);
}

void StressGauge::TakeDamage(float amount) {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (isGodMode_) {
        return;
    }
#endif
    currentStress_ = (std::min)(currentStress_ + amount, maxStress_);
}

void StressGauge::OnEntourageEnemyHit() {
    currentStress_ = (std::max)(0.0f, currentStress_ - stressReductionPerHit_);
    if (onDecayTick_) {
        onDecayTick_();
    }
}

void StressGauge::RegisterParams() {
    globalParameter_->Regist(groupName_, "maxStress",            &maxStress_);
    globalParameter_->Regist(groupName_, "stressReductionPerHit", &stressReductionPerHit_);
}

void StressGauge::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("最大ストレス", &maxStress_, 0.1f, 1.0f, 10000.0f);
        ImGui::SeparatorText("ストレス減少");
        ImGui::DragFloat("ザコ攻撃1回の減少量", &stressReductionPerHit_, 0.1f, 0.0f, 100.0f);

        ImGui::Separator();
        if (ImGui::Button(isGodMode_ ? "[DEBUG] ストレス増加: 停止中 → 再開" : "[DEBUG] ストレス増加: 動作中 → 停止")) {
            isGodMode_ = !isGodMode_;
        }
        if (isGodMode_) {
            ImGui::SameLine();
            ImGui::TextColored({0.4f, 1.0f, 0.4f, 1.0f}, "GOD MODE ON");
        }

        ImGui::Separator();
        ImGui::Text("現在のストレス: %.1f / %.1f", currentStress_, maxStress_);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    StressGaugeGauge_->AdjustParam();
#endif
}
