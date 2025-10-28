#include "DissolveData.h"
#include <algorithm>
#include <imgui.h>
#include <Windows.h>

void DissolveData::Init(const std::string& dissolveName) {
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = dissolveName;
    globalParameter_->CreateGroup(groupName_, true);

    // 重複バインドを防ぐ
    if (!globalParameter_->HasBindings(groupName_)) {
        BindParams();
    }

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);

    // イージング設定
    thresholdEase_.SetAdaptValue(&easedThreshold_);
    thresholdEase_.SetStartValue(startThreshold_);
    thresholdEase_.SetEndValue(endThreshold_);

    thresholdEase_.SetOnFinishCallback([this]() {
        Stop();
        Reset();
    });

    // 初期状態
    playState_        = PlayState::STOPPED;
    currentThreshold_ = startThreshold_;
    currentEnable_    = false;
}

void DissolveData::Update(float deltaTime) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    currentTime_ += deltaTime;

    // オフセット時間をチェック
    if (currentTime_ < offsetTime_) {
        // オフセット時間内
        currentThreshold_ = startThreshold_;
        currentEnable_    = (startThreshold_ < 1.0f);
        return;
    }

    // イージング更新
    thresholdEase_.SetMaxTime(maxTime_);
    thresholdEase_.SetType(static_cast<EasingType>(easeType_));
    thresholdEase_.Update(deltaTime);

    // ディゾルブ値計算
    UpdateDissolveValues();
}

void DissolveData::UpdateDissolveValues() {
    currentThreshold_ = easedThreshold_;

    // ディゾルブが有効かどうかを判定
    currentEnable_ = (currentThreshold_ < 1.0f);
}

void DissolveData::Play() {
    Reset();

    playState_ = PlayState::PLAYING;
    totalTime_ = offsetTime_ + maxTime_;

    // イージング初期化
    thresholdEase_.SetStartValue(startThreshold_);
    thresholdEase_.SetEndValue(endThreshold_);
    thresholdEase_.SetMaxTime(maxTime_);
    thresholdEase_.SetType(static_cast<EasingType>(easeType_));
    thresholdEase_.Reset();
    easedThreshold_ = startThreshold_;

    // 初期値設定
    currentThreshold_ = startThreshold_;
    currentEnable_    = (startThreshold_ < 1.0f);
}

void DissolveData::Stop() {
    playState_        = PlayState::STOPPED;
    currentThreshold_ = startThreshold_;
    currentEnable_    = false;
}

void DissolveData::Reset() {
    currentTime_      = 0.0f;
    easedThreshold_   = startThreshold_;
    currentThreshold_ = startThreshold_;
    currentEnable_    = false;

    thresholdEase_.Reset();
}

bool DissolveData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

void DissolveData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void DissolveData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void DissolveData::BindParams() {
    globalParameter_->Bind(groupName_, "startThreshold", &startThreshold_);
    globalParameter_->Bind(groupName_, "endThreshold", &endThreshold_);
    globalParameter_->Bind(groupName_, "maxTime", &maxTime_);
    globalParameter_->Bind(groupName_, "offsetTime", &offsetTime_);
    globalParameter_->Bind(groupName_, "easeType", &easeType_);
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
        if (ImGui::Button("Stop"))
            Stop();
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
        }
        ImGui::Text("State: %s", stateText);

        // 進行状況を表示
        float progress = 0.0f;
        if (totalTime_ > 0.0f) {
            progress = std::clamp(currentTime_ / totalTime_, 0.0f, 1.0f);
        }
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Progress");

        // 現在の値を表示
        ImGui::Text("Current Threshold: %.3f", currentThreshold_);
        ImGui::Text("Current Enabled: %s", currentEnable_ ? "TRUE" : "FALSE");

        ImGui::Separator();

        // Threshold値設定
        ImGui::DragFloat("Start Threshold", &startThreshold_, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("End Threshold", &endThreshold_, 0.01f, 0.0f, 1.0f);

        ImGui::Separator();

        // タイミング設定
        ImGui::DragFloat("Max Time", &maxTime_, 0.01f, 0.1f, 10.0f);
        ImGui::DragFloat("Offset Time", &offsetTime_, 0.01f, 0.0f, 5.0f);

        // イージングタイプ
        EasingTypeSelector("Easing Type", easeType_);

        ImGui::Separator();

        // セーブ・ロード
        if (ImGui::Button("Load Data")) {
            LoadData();
            MessageBoxA(nullptr, "Dissolve data loaded successfully.", "Dissolve Data", 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save Data")) {
            SaveData();
            MessageBoxA(nullptr, "Dissolve data saved successfully.", "Dissolve Data", 0);
        }

        ImGui::PopID();
    }
#endif // _DEBUG
}

void DissolveData::EasingTypeSelector(const char* label, int32_t& target) {
    int type = static_cast<int32_t>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;
    }
}

bool DissolveData::IsPlaying() const {
    return playState_ == PlayState::PLAYING;
}