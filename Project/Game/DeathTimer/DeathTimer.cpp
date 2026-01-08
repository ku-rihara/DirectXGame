#include "DeathTimer.h"
#include "Frame/Frame.h"
#include <imgui.h>
#include "Easing/EasingFunction.h"

void DeathTimer::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    deathTimerGauge_ = std::make_unique<DeathTimerGauge>();
    deathTimerGauge_->Init();

    //タイマー初期化
    currentTimer_ = maxTimer_;
}

void DeathTimer::Update(float timer) {
   
    // イージング適応
    AdaptEasing(timer);

    // maxTimerを超えないようにクランプ
    currentTimer_ = std::clamp(currentTimer_, 0.0f, maxTimer_);

    // タイムセット
    deathTimerGauge_->Update(timer);
    deathTimerGauge_->SetTimer(currentTimer_, maxTimer_);
}

void DeathTimer::IncrementTimer() {
    if (isIncrementing_) {
        // 既にイージング中の場合、目標値を更新
        incrementTargetValue_ += incrementTime_;
    } else {
        // 新規イージング開始
        incrementStartValue_  = currentTimer_;
        incrementTargetValue_ = currentTimer_ + incrementTime_;
        incrementTimer_       = 0.0f;
        isIncrementing_       = true;
    }

    // maxTimerを超えないようにクランプ
    incrementTargetValue_ = (std::min)(incrementTargetValue_, maxTimer_);
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
        ImGui::DragFloat("maxTimer", &maxTimer_, 0.01f);
        ImGui::DragFloat("incrementDuration", &incrementDuration_, 0.01f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

    deathTimerGauge_->AdjustParam();

#endif // _DEBUG
}

void DeathTimer::RegisterParams() {
    globalParameter_->Regist(groupName_, "decrementSpeedRate", &decrementSpeedRate_);
    globalParameter_->Regist(groupName_, "incrementTime", &incrementTime_);
    globalParameter_->Regist(groupName_, "maxTimer", &maxTimer_);
    globalParameter_->Regist(groupName_, "incrementDuration", &incrementDuration_);
}

void DeathTimer::AdaptEasing(float timeSpeed) {
    // イージング中の処理
    if (isIncrementing_) {
        incrementTimer_ += KetaEngine::Frame::DeltaTimeRate();

        // イージングでタイマーを増加
        currentTimer_ = EaseOutQuad(
            incrementStartValue_,
            incrementTargetValue_,
            incrementTimer_,
            incrementDuration_);

        // イージング完了
        if (incrementTimer_ >= incrementDuration_) {
            currentTimer_   = incrementTargetValue_;
            isIncrementing_ = false;
            incrementTimer_ = 0.0f;
        }
    } else {
        // 通常の減少処理
        currentTimer_ -= timeSpeed * decrementSpeedRate_;
    }
}