#include "DeathTimer.h"
#include "Frame/Frame.h"
#include <algorithm>
#include <imgui.h>

void DeathTimer::Init() {

    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    deathTimerGauge_ = std::make_unique<DeathTimerGauge>();
    deathTimerGauge_->Init();
    deathTimerGauge_->SetTimer(0.0f, maxStress_);

    currentStress_  = 0.0f;
    tauntTickTimer_ = 0.0f;
    isDeath_        = false;
}

void DeathTimer::Update(float deltaTime) {

    if (!isGodMode_) {
        // 煽り中は秒間隔でストレス増加
        if (isTaunting_) {
            tauntTickTimer_ += deltaTime;
            if (tauntTickTimer_ >= tauntTickInterval_) {
                tauntTickTimer_ -= tauntTickInterval_;
                float amount = baseStressRate_ + static_cast<float>(tauntingEnemyCount_) * stressRatePerEnemy_;
                currentStress_ += amount;
            }
        }
    }

    currentStress_ = std::clamp(currentStress_, 0.0f, maxStress_);

    if (currentStress_ >= maxStress_) {
        isDeath_ = true;
    }

    // SetTimer で目標値を先に更新し、Update でイージング処理する
    deathTimerGauge_->SetTimer(currentStress_, maxStress_);
    deathTimerGauge_->Update(deltaTime);
}

void DeathTimer::TakeDamage(float amount) {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (isGodMode_) {
        return;
    }
#endif
    currentStress_ = (std::min)(currentStress_ + amount, maxStress_);
}

void DeathTimer::SetTauntState(bool isTaunting, int32_t tauntingCount) {
    isTaunting_         = isTaunting;
    tauntingEnemyCount_ = tauntingCount;
}

void DeathTimer::OnNormalEnemyHit() {
    currentStress_ = (std::max)(0.0f, currentStress_ - stressReductionPerHit_);
    if (onDecayTick_) {
        onDecayTick_();
    }
}

void DeathTimer::RegisterParams() {
    globalParameter_->Regist(groupName_, "maxStress", &maxStress_);
    globalParameter_->Regist(groupName_, "tauntTickInterval", &tauntTickInterval_);
    globalParameter_->Regist(groupName_, "baseStressRate", &baseStressRate_);
    globalParameter_->Regist(groupName_, "stressRatePerEnemy", &stressRatePerEnemy_);
    globalParameter_->Regist(groupName_, "stressReductionPerHit", &stressReductionPerHit_);
}

void DeathTimer::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("最大ストレス", &maxStress_, 0.1f, 1.0f, 10000.0f);
        ImGui::SeparatorText("ストレス増加");
        ImGui::DragFloat("増加間隔（秒）", &tauntTickInterval_, 0.05f, 0.1f, 30.0f);
        ImGui::DragFloat("基準増加量（秒ごと）", &baseStressRate_, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("ザコ1体あたり追加増加量", &stressRatePerEnemy_, 0.1f, 0.0f, 50.0f);
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
        ImGui::Text("煽り中: %s  煽りザコ数: %d", isTaunting_ ? "YES" : "NO", tauntingEnemyCount_);

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    deathTimerGauge_->AdjustParam();
#endif
}
