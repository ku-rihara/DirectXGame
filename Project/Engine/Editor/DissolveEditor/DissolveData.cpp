#include "DissolveData.h"

using namespace KetaEngine;
// Parameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// Frame
#include "Frame/Frame.h"
// std
#include <algorithm>
#include <filesystem>
#include <imgui.h>

void DissolveData::Init(const std::string& dissolveName, const std::string& categoryName) {
    BaseEffectData::Init(dissolveName, categoryName);

    groupName_  = dissolveName;
    folderPath_ = baseFolderPath_ + categoryName_ + "/" + "Dates";

    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
        InitParams();
    } else {
        GetParams();
    }

    // イージング設定
    thresholdEase_.SetAdaptValue(&easedThreshold_);
    thresholdEase_.SetStartValue(startThreshold_);
    thresholdEase_.SetEndValue(endThreshold_);

    thresholdEase_.SetOnFinishCallback([this]() {
        playState_ = PlayState::STOPPED;
        Reset();
    });

    playState_        = PlayState::STOPPED;
    currentThreshold_ = startThreshold_;
    currentEnable_    = false;
}

void DissolveData::Update(float speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    float deltaTime = Frame::DeltaTimeRate() * speedRate;
    currentTime_ += deltaTime;

    if (currentTime_ < offsetTime_) {
        currentThreshold_ = startThreshold_;
        currentEnable_    = (startThreshold_ < 1.0f);
        return;
    }

    thresholdEase_.SetMaxTime(maxTime_);
    thresholdEase_.SetType(static_cast<EasingType>(easeType_));
    thresholdEase_.Update(deltaTime);

    UpdateDissolveValues();
}

void DissolveData::UpdateDissolveValues() {
    currentThreshold_ = easedThreshold_;
    currentEnable_    = (currentThreshold_ < 1.0f);
}

void DissolveData::Play() {
    Reset();

    playState_ = PlayState::PLAYING;
    totalTime_ = offsetTime_ + maxTime_;

    thresholdEase_.SetStartValue(startThreshold_);
    thresholdEase_.SetEndValue(endThreshold_);
    thresholdEase_.SetMaxTime(maxTime_);
    thresholdEase_.SetType(static_cast<EasingType>(easeType_));
    thresholdEase_.Reset();
    easedThreshold_ = startThreshold_;

    currentThreshold_ = startThreshold_;
    currentEnable_    = (startThreshold_ < 1.0f);
}

void DissolveData::Reset() {
    currentTime_      = 0.0f;
    easedThreshold_   = startThreshold_;
    currentThreshold_ = startThreshold_;
    currentEnable_    = false;
    thresholdEase_.Reset();
}

void DissolveData::RegisterParams() {
    globalParameter_->Regist(groupName_, "startThreshold",    &startThreshold_);
    globalParameter_->Regist(groupName_, "endThreshold",      &endThreshold_);
    globalParameter_->Regist(groupName_, "maxTime",           &maxTime_);
    globalParameter_->Regist(groupName_, "offsetTime",        &offsetTime_);
    globalParameter_->Regist(groupName_, "easeType",          &easeType_);
    globalParameter_->Regist(groupName_, "currentTexturePath",&currentTexturePath_);
}

void DissolveData::GetParams() {
    if (!globalParameter_->HasGroup(groupName_)) {
        return;
    }

    startThreshold_     = globalParameter_->GetValue<float>(groupName_, "startThreshold");
    endThreshold_       = globalParameter_->GetValue<float>(groupName_, "endThreshold");
    maxTime_            = globalParameter_->GetValue<float>(groupName_, "maxTime");
    offsetTime_         = globalParameter_->GetValue<float>(groupName_, "offsetTime");
    easeType_           = globalParameter_->GetValue<int32_t>(groupName_, "easeType");
    currentTexturePath_ = globalParameter_->GetValue<std::string>(groupName_, "currentTexturePath");
}

void DissolveData::InitParams() {
    startThreshold_     = 1.0f;
    endThreshold_       = 0.0f;
    maxTime_            = 1.0f;
    offsetTime_         = 0.0f;
    easeType_           = 0;
    currentTexturePath_ = "";
}

void DissolveData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Dissolve Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // 再生制御
        DrawPlayButton();

        // 状態表示
        const char* stateText = "";
        switch (playState_) {
        case PlayState::STOPPED:   stateText = "STOPPED";   break;
        case PlayState::PLAYING:   stateText = "PLAYING";   break;
        case PlayState::PAUSED:    stateText = "PAUSED";    break;
        case PlayState::RETURNING: stateText = "RETURNING"; break;
        }
        ImGui::Text("State: %s", stateText);

        // 進行状況
        float progress = 0.0f;
        if (totalTime_ > 0.0f) {
            progress = std::clamp(currentTime_ / totalTime_, 0.0f, 1.0f);
        }
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Progress");

        ImGui::Text("Current Threshold: %.3f", currentThreshold_);
        ImGui::Text("Current Enabled: %s", currentEnable_ ? "TRUE" : "FALSE");

        ImGui::Separator();

        // Threshold設定
        ImGui::DragFloat("Start Threshold", &startThreshold_, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("End Threshold",   &endThreshold_,   0.01f, 0.0f, 1.0f);

        ImGui::Separator();

        // タイミング設定
        ImGui::DragFloat("Max Time",    &maxTime_,    0.01f, 0.1f, 10.0f);
        ImGui::DragFloat("Offset Time", &offsetTime_, 0.01f, 0.0f, 5.0f);

        // イージングタイプ
        ImGuiEasingTypeSelector("Easing Type", easeType_);

        ImGui::Separator();

        // テクスチャ選択（FileSelector使用）
        ImGui::Text("Noise Texture:");
        textureSelector_.SelectFilePath("##NoiseTexture", textureFilePath_, currentTexturePath_, ".dds", false);
        if (!currentTexturePath_.empty()) {
            ImGui::TextDisabled("Path: %s", currentTexturePath_.c_str());
        }

        ImGui::PopID();
    }
#endif
}
