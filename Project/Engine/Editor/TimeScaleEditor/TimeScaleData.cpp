#include "TimeScaleData.h"

using namespace KetaEngine;
#include "Frame/Frame.h"
#include <imgui.h>
#include <Windows.h>

void TimeScaleData::Init(const std::string& timeScaleName, const std::string& categoryName) {
    BaseEffectData::Init(timeScaleName, categoryName);

    groupName_  = timeScaleName;
    folderPath_ = baseFolderPath_ + categoryName_ + "/" + "Dates";

    if (!globalParameter_->HasRegisters(groupName_)) {
        // 新規登録
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
    } else {
        // 値取得
        GetParams();
    }

    // 初期化
    InitParams();
}

void TimeScaleData::Update(float deltaTime) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    // 経過時間更新
    elapsedTime_ += deltaTime;

    // 時間が経過したらTimeScaleを戻す
    if (elapsedTime_ >= duration_) {
        playState_ = PlayState::STOPPED;
        ResetTimeScale();
    }
}

void TimeScaleData::Play() {
    Reset();
    playState_ = PlayState::PLAYING;
    ApplyTimeScale();
}

void TimeScaleData::Reset() {
    elapsedTime_ = 0.0f;
    playState_   = PlayState::STOPPED;
    ResetTimeScale();
}

void TimeScaleData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
    GetParams();
}

void TimeScaleData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void TimeScaleData::RegisterParams() {
    globalParameter_->Regist(groupName_, "timeScale", &timeScale_);
    globalParameter_->Regist(groupName_, "duration", &duration_);
    timeModeSelector_.RegisterParam(groupName_, globalParameter_);
}

void TimeScaleData::GetParams() {
    timeScale_ = globalParameter_->GetValue<float>(groupName_, "timeScale");
    duration_  = globalParameter_->GetValue<float>(groupName_, "duration");
    timeModeSelector_.GetParam(groupName_, globalParameter_);
}

void TimeScaleData::InitParams() {
    elapsedTime_ = 0.0f;
    playState_   = PlayState::STOPPED;
}

void TimeScaleData::ApplyTimeScale() {
    Frame::SetTimeScale(timeScale_);
}

void TimeScaleData::ResetTimeScale() {
    Frame::SetTimeScale(1.0f);
}

void TimeScaleData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("TimeScale Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // 状態表示
        const char* stateText = "";
        switch (playState_) {
        case PlayState::STOPPED:
            stateText = "STOPPED";
            break;
        case PlayState::PLAYING:
            stateText = "PLAYING";
            break;
        case PlayState::PAUSED:
            stateText = "PAUSED";
            break;
        case PlayState::RETURNING:
            stateText = "RETURNING";
            break;
        }
        ImGui::Text("State: %s", stateText);

        // 進行状況を表示
        float progress = 0.0f;
        if (duration_ > 0.0f) {
            progress = elapsedTime_ / duration_;
        }
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Progress");
        ImGui::Text("Time: %.2f / %.2f (%.2fs remaining)",
            elapsedTime_, duration_, GetRemainingTime());

        ImGui::Separator();

        // パラメータ調整
        ImGui::DragFloat("TimeScale", &timeScale_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Duration", &duration_, 0.01f, 0.1f, 10.0f);

        ImGui::Separator();

        // タイムモード設定
        timeModeSelector_.SelectTimeModeImGui("Time Mode");

        ImGui::PopID();
    }
#endif // _DEBUG
}