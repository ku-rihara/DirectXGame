#include "DeathTimer.h"
#include "Easing/EasingFunction.h"
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

    // HP初期化
    currentHP_ = maxHP_;
}

void DeathTimer::Update(float timer) {

    // イージング適応
    AdaptEasing();

    // maxHPを超えないようにクランプ
    currentHP_ = std::clamp(currentHP_, 0.0f, maxHP_);
    if (currentHP_ <= 0.0f) {
        isDeath_ = true;
    }

    // ゲージ更新
    deathTimerGauge_->Update(timer);
    deathTimerGauge_->SetTimer(currentHP_, maxHP_);
}

void DeathTimer::TakeDamage(float damage) {
    // ダメージを受ける
    currentHP_ -= damage;

    // 0以下にならないようにクランプ
    if (currentHP_ < 0.0f) {
        currentHP_ = 0.0f;
    }
}

void DeathTimer::RecoverHP() {
    if (isRecovering_) {
        // 既にイージング中の場合、目標値を更新
        recoveryTargetValue_ += levelUpRecoveryAmount_;
    } else {
        // 新規イージング開始
        recoveryStartValue_  = currentHP_;
        recoveryTargetValue_ = currentHP_ + levelUpRecoveryAmount_;
        recoveryTimer_       = 0.0f;
        isRecovering_        = true;
    }

    // maxHPを超えないようにクランプ
    recoveryTargetValue_ = (std::min)(recoveryTargetValue_, maxHP_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void DeathTimer::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("最大HP", &maxHP_, 0.01f);
        ImGui::DragFloat("レベルアップ回復量", &levelUpRecoveryAmount_, 0.01f);
        ImGui::DragFloat("回復イージング時間", &recoveryDuration_, 0.01f);

        ImGui::Separator();
        ImGui::Text("現在のHP: %.2f / %.2f", currentHP_, maxHP_);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    deathTimerGauge_->AdjustParam();

#endif // _DEBUG
}

void DeathTimer::RegisterParams() {
    globalParameter_->Regist(groupName_, "maxHP", &maxHP_);
    globalParameter_->Regist(groupName_, "levelUpRecoveryAmount", &levelUpRecoveryAmount_);
    globalParameter_->Regist(groupName_, "recoveryDuration", &recoveryDuration_);
}

void DeathTimer::AdaptEasing() {
    // イージング中の処理（HP回復時）
    if (isRecovering_) {
        recoveryTimer_ += KetaEngine::Frame::DeltaTimeRate();

        // イージングでHPを増加
        currentHP_ = EaseOutQuad(
            recoveryStartValue_,
            recoveryTargetValue_,
            recoveryTimer_,
            recoveryDuration_);

        // イージング完了
        if (recoveryTimer_ >= recoveryDuration_) {
            currentHP_      = recoveryTargetValue_;
            isRecovering_   = false;
            recoveryTimer_  = 0.0f;
        }
    }
    // 時間経過による減少は削除（敵からのダメージのみ）
}
