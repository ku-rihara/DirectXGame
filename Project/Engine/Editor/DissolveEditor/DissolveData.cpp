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
    } else {
        GetParams();
    }

    InitParams();

    LoadNoiseTextures();

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
    globalParameter_->Regist(groupName_, "startThreshold", &startThreshold_);
    globalParameter_->Regist(groupName_, "endThreshold", &endThreshold_);
    globalParameter_->Regist(groupName_, "maxTime", &maxTime_);
    globalParameter_->Regist(groupName_, "offsetTime", &offsetTime_);
    globalParameter_->Regist(groupName_, "easeType", &easeType_);
    globalParameter_->Regist(groupName_, "selectedTextureIndex", &selectedTextureIndex_);
}

void DissolveData::GetParams() {

    if (selectedTextureIndex_ >= 0 && selectedTextureIndex_ < static_cast<int32_t>(noiseTextureFiles_.size())) {
        currentTexturePath_ = textureFilePath_ + "/" + noiseTextureFiles_[selectedTextureIndex_];
    }
}

void DissolveData::InitParams() {
    startThreshold_       = 1.0f;
    endThreshold_         = 0.0f;
    maxTime_              = 1.0f;
    offsetTime_           = 0.0f;
    easeType_             = 0;
    selectedTextureIndex_ = 0;
}

void DissolveData::LoadNoiseTextures() {
    noiseTextureFiles_.clear();

    if (!std::filesystem::exists(textureFilePath_)) {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(textureFilePath_)) {
        if (entry.is_regular_file()) {
            noiseTextureFiles_.push_back(entry.path().filename().string());
        }
    }

    std::sort(noiseTextureFiles_.begin(), noiseTextureFiles_.end());

    if (!noiseTextureFiles_.empty() && selectedTextureIndex_ >= 0 && selectedTextureIndex_ < static_cast<int32_t>(noiseTextureFiles_.size())) {
        currentTexturePath_ = textureFilePath_ + "/" + noiseTextureFiles_[selectedTextureIndex_];
    }
}

void DissolveData::SetTextureIndex(int32_t index) {
    if (index >= 0 && index < static_cast<int32_t>(noiseTextureFiles_.size())) {
        selectedTextureIndex_ = index;
        currentTexturePath_   = textureFilePath_ + "/" + noiseTextureFiles_[selectedTextureIndex_];
    }
}

void DissolveData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Dissolve Editor: " + groupName_).c_str());
        ImGui::PushID(groupName_.c_str());

        // 再生制御
        if (ImGui::Button("Play"))
            Play();
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
            Reset();

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
        ImGui::DragFloat("End Threshold", &endThreshold_, 0.01f, 0.0f, 1.0f);

        ImGui::Separator();

        // タイミング設定
        ImGui::DragFloat("Max Time", &maxTime_, 0.01f, 0.1f, 10.0f);
        ImGui::DragFloat("Offset Time", &offsetTime_, 0.01f, 0.0f, 5.0f);

        // イージングタイプ
        ImGuiEasingTypeSelector("Easing Type", easeType_);

        ImGui::Separator();

        // テクスチャ選択
        if (!noiseTextureFiles_.empty()) {
            ImGui::Text("Noise Texture:");
            if (ImGui::BeginCombo("##NoiseTexture", noiseTextureFiles_[selectedTextureIndex_].c_str())) {
                for (int32_t i = 0; i < static_cast<int32_t>(noiseTextureFiles_.size()); i++) {
                    bool isSelected = (selectedTextureIndex_ == i);
                    if (ImGui::Selectable(noiseTextureFiles_[i].c_str(), isSelected)) {
                        SetTextureIndex(i);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Current: %s", currentTexturePath_.c_str());
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No noise textures found!");
        }

        ImGui::PopID();
    }
#endif
}