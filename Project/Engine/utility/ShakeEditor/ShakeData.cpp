#include "ShakeData.h"
#include "Shake.h"
#include <algorithm>
#include <imgui.h>
#include <Windows.h>

void ShakeData::Init(const std::string& shakeName) {
    globalParameter_ = GlobalParameter::GetInstance();

    // グループ名設定
    groupName_ = shakeName;
    globalParameter_->CreateGroup(groupName_, true);

    // 重複バインドを防ぐ
    globalParameter_->ClearBindingsForGroup(groupName_);
    BindParams();

    // パラメータ同期
    globalParameter_->SyncParamForGroup(groupName_);

    // イージング設定
    timeEase_.SetAdaptValue(&easedTime_);
    timeEase_.SetStartValue(startTime_);
    timeEase_.SetEndValue(0.0f);

     timeEase_.SetOnFinishCallback([this]() {
        Stop();
        Reset();
    });

    // 初期状態
    playState_          = PlayState::STOPPED;
    currentShakeOffset_ = {0.0f, 0.0f, 0.0f};
}

void ShakeData::Update(const float& deltaTime) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    // イージング更新
    timeEase_.SetMaxTime(maxTime_);
    timeEase_.SetType(static_cast<EasingType>(easeType_));
    timeEase_.Update(deltaTime);

    // シェイク値計算
    UpdateShakeValues();
}

void ShakeData::UpdateShakeValues() {
    UpdateVector3Shake();
}

void ShakeData::UpdateVector3Shake() {
    float t = easedTime_;

    Vector3 shakeVector;
    if (shakeType_ == static_cast<int32_t>(ShakeType::WAVE)) {
        shakeVector = ShakeWave<Vector3>(t, shakeLength_);
    } else {
        shakeVector = Shake<Vector3>(t, shakeLength_);
    }

    // 軸フラグを適用
    currentShakeOffset_ = ApplyAxisFlag(shakeVector);
}

Vector3 ShakeData::ApplyAxisFlag(const Vector3& shakeValue) const {
    int32_t flag   = axisFlag_;
    Vector3 result = {0.0f, 0.0f, 0.0f};

    if (flag & AXIS_X) {
        result.x = shakeValue.x;
    }
    if (flag & AXIS_Y) {
        result.y = shakeValue.y;
    }
    if (flag & AXIS_Z) {
        result.z = shakeValue.z;
    }

    return result;
}

void ShakeData::Play() {
    Reset();

    playState_ = PlayState::PLAYING;

    // イージング初期化
    timeEase_.SetStartValue(startTime_);
    timeEase_.SetEndValue(0.0f);
    timeEase_.SetMaxTime(maxTime_);
    timeEase_.SetType(static_cast<EasingType>(easeType_));
    timeEase_.Reset();
    easedTime_ = startTime_;

}

void ShakeData::Stop() {
    playState_          = PlayState::STOPPED;
    currentShakeOffset_ = {0.0f, 0.0f, 0.0f};
}

void ShakeData::Reset() {

    easedTime_          = startTime_;
    currentShakeOffset_ = {0.0f, 0.0f, 0.0f};

    timeEase_.Reset();
}

bool ShakeData::IsFinished() const {
    return playState_ == PlayState::STOPPED;
}

void ShakeData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void ShakeData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void ShakeData::BindParams() {
    globalParameter_->Bind(groupName_, "shakeLength", &shakeLength_);
    globalParameter_->Bind(groupName_, "maxTime", &maxTime_);
    globalParameter_->Bind(groupName_, "easeType", &easeType_);
    globalParameter_->Bind(groupName_, "shakeType", &shakeType_);
    globalParameter_->Bind(groupName_, "startTime", &startTime_);
    globalParameter_->Bind(groupName_, "axisFlag", &axisFlag_);
}

void ShakeData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Shake Editor: " + groupName_).c_str());
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
        if (maxTime_ > 0.0f) {
            progress = (startTime_ - easedTime_) / (startTime_ - 0.0f);
        }
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Progress");

        ImGui::Separator();

        bool x = (axisFlag_ & AXIS_X) != 0;
        bool y = (axisFlag_ & AXIS_Y) != 0;
        bool z = (axisFlag_ & AXIS_Z) != 0;

        ImGui::Text("Active Axes:");
        if (ImGui::Checkbox("X", &x)) {
            if (x)
                axisFlag_ |= AXIS_X;
            else
                axisFlag_ &= ~AXIS_X;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Y", &y)) {
            if (y)
                axisFlag_ |= AXIS_Y;
            else
                axisFlag_ &= ~AXIS_Y;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Z", &z)) {
            if (z)
                axisFlag_ |= AXIS_Z;
            else
                axisFlag_ &= ~AXIS_Z;
        }

        ImGui::Separator();

        // パラメータ調整
        ImGui::DragFloat("Shake Length", &shakeLength_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Max Time", &maxTime_, 0.01f, 0.1f, 10.0f);
        ImGui::DragFloat("startTime", &startTime_, 0.01f, 0.1f, 1.0f);

        // シェイクタイプ
        const char* shakeTypeItems[] = {"Normal", "Wave"};
        ImGui::Combo("Shake Type", &shakeType_, shakeTypeItems, IM_ARRAYSIZE(shakeTypeItems));

        // イージングタイプ
        EasingTypeSelector("Easing Type", easeType_);

        ImGui::Separator();

        // Load ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
        if (ImGui::Button("Load ShakeData")) {
            LoadData();
            MessageBoxA(nullptr, "shakeData loaded successfully.", "ShakeData", 0);
        }
        ImGui::PopStyleColor(3);

        // Save ボタン
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
        if (ImGui::Button("Save ShakeData")) {
            SaveData();
            MessageBoxA(nullptr, "All animations saved successfully.", "ShakeData", 0);
        }
        ImGui::PopStyleColor(3);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void ShakeData::EasingTypeSelector(const char* label, int32_t& target) {
    int type = static_cast<int32_t>(target);
    if (ImGui::Combo(label, &type, EasingTypeLabels.data(), static_cast<int>(EasingTypeLabels.size()))) {
        target = type;
    }
}

bool ShakeData::IsPlaying() const {
    return playState_ == PlayState::PLAYING;
}