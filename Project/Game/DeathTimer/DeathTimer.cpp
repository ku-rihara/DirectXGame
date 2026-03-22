#include "DeathTimer.h"
#include "Easing/EasingFunction.h"
#include "Frame/Frame.h"
#include <cmath>
#include <imgui.h>

void DeathTimer::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    deathTimerGauge_ = std::make_unique<DeathTimerGauge>();
    deathTimerGauge_->Init();

    levelUIController_.Init();

    // HP初期化
    currentHP_    = maxHP_;
    currentLevel_ = 1;
    killCount_    = 0;
}

void DeathTimer::Update(float deltaTime) {

    // イージング適応
    AdaptEasing();
    TakeDamage(deltaTime);

    // maxHPを超えないようにクランプ
    currentHP_ = std::clamp(currentHP_, 0.0f, maxHP_);
    if (currentHP_ <= 0.0f) {
        isDeath_ = true;
    }

    // ゲージ更新
    deathTimerGauge_->Update(deltaTime);
    deathTimerGauge_->SetTimer(currentHP_, maxHP_);

    levelUIController_.Update(currentLevel_);
}

void DeathTimer::TakeDamage(float deltaTime) {
    currentHP_ -= decreaseRates_[currentLevel_ - 1] * deltaTime;
    if (currentHP_ < 0.0f) {
        currentHP_ = 0.0f;
    }
}

void DeathTimer::OnEnemyKilled(float gaugeAmount, int32_t comboCount) {
    killCount_++;
    UpdateLevel();

    // コンボ倍率を計算して回復量に乗算
    int32_t step     = (comboStepSize_ > 0) ? (comboCount / comboStepSize_) : 0;
    float multiplier = (std::min)(std::pow(comboMultiplierPerStep_, static_cast<float>(step)), comboMaxMultiplier_);
    RecoverHP(gaugeAmount * multiplier);

    if (onKillCallback_) {
        onKillCallback_(static_cast<int32_t>(multiplier));
    }
}

void DeathTimer::ApplyBonus(float amount) {
    RecoverHP(amount);
}

void DeathTimer::RecoverHP(float amount) {
    if (isRecovering_) {
        recoveryTargetValue_ += amount;
    } else {
        recoveryStartValue_  = currentHP_;
        recoveryTargetValue_ = currentHP_ + amount;
        recoveryTimer_       = 0.0f;
        isRecovering_        = true;
    }
    recoveryTargetValue_ = (std::min)(recoveryTargetValue_, maxHP_);
}

void DeathTimer::UpdateLevel() {
    int32_t newLevel = 1;
    for (int32_t i = 0; i < kMaxLevel - 1; ++i) {
        if (killCount_ >= levelUpKillCounts_[i]) {
            newLevel = i + 2;
        }
    }
    currentLevel_ = newLevel;
}

///==========================================================
/// パラメータ調整
///==========================================================
void DeathTimer::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("最大HP", &maxHP_, 0.01f);
        ImGui::DragFloat("回復イージング時間", &recoveryDuration_, 0.01f);

        ImGui::SeparatorText("ゲージ減少レート（レベル別）");
        for (int32_t i = 0; i < kMaxLevel; ++i) {
            std::string label = "Lv" + std::to_string(i + 1) + " 減少レート";
            ImGui::DragFloat(label.c_str(), &decreaseRates_[i], 0.001f, 0.0f, 10.0f);
        }

        ImGui::SeparatorText("コンボ倍率設定");
        ImGui::InputInt("コンボステップ数（N コンボおきに倍率UP）", &comboStepSize_);
        ImGui::DragFloat("1ステップごとの倍率", &comboMultiplierPerStep_, 0.01f, 1.0f, 10.0f);
        ImGui::DragFloat("倍率の上限", &comboMaxMultiplier_, 0.01f, 1.0f, 100.0f);

        ImGui::SeparatorText("レベルアップ閾値（累計キル数）");
        for (int32_t i = 0; i < kMaxLevel - 1; ++i) {
            std::string label = "Lv" + std::to_string(i + 2) + " 到達キル数";
            ImGui::InputInt(label.c_str(), &levelUpKillCounts_[i]);
        }

        ImGui::Separator();
        ImGui::Text("現在のHP: %.2f / %.2f", currentHP_, maxHP_);
        ImGui::Text("現在のレベル: %d / %d", currentLevel_, kMaxLevel);
        ImGui::Text("累計キル数: %d", killCount_);
        // コンボ倍率表示
        if (comboStepSize_ > 0) {
            ImGui::Text("倍率ステップ: 0コンボ=x1.00 / %dコンボ=x%.2f / %dコンボ=x%.2f / %dコンボ=x%.2f",
                comboStepSize_,  std::pow(comboMultiplierPerStep_, 1.0f),
                comboStepSize_ * 2, std::pow(comboMultiplierPerStep_, 2.0f),
                comboStepSize_ * 3, std::pow(comboMultiplierPerStep_, 3.0f));
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    deathTimerGauge_->AdjustParam();
    levelUIController_.AdjustParam();

#endif // _DEBUG
}

void DeathTimer::RegisterParams() {
    globalParameter_->Regist(groupName_, "maxHP", &maxHP_);
    globalParameter_->Regist(groupName_, "recoveryDuration", &recoveryDuration_);
    globalParameter_->Regist(groupName_, "comboStepSize", &comboStepSize_);
    globalParameter_->Regist(groupName_, "comboMultiplierPerStep", &comboMultiplierPerStep_);
    globalParameter_->Regist(groupName_, "comboMaxMultiplier", &comboMaxMultiplier_);

    for (int32_t i = 0; i < kMaxLevel; ++i) {
        globalParameter_->Regist(groupName_, "decreaseRate" + std::to_string(i + 1), &decreaseRates_[i]);
    }
    for (int32_t i = 0; i < kMaxLevel - 1; ++i) {
        globalParameter_->Regist(groupName_, "levelUpKillCount" + std::to_string(i + 2), &levelUpKillCounts_[i]);
    }
}

void DeathTimer::AdaptEasing() {
    if (isRecovering_) {
        recoveryTimer_ += KetaEngine::Frame::DeltaTimeRate();

        currentHP_ = EaseOutQuad(
            recoveryStartValue_,
            recoveryTargetValue_,
            recoveryTimer_,
            recoveryDuration_);

        if (recoveryTimer_ >= recoveryDuration_) {
            currentHP_     = recoveryTargetValue_;
            isRecovering_  = false;
            recoveryTimer_ = 0.0f;
        }
    }
}
