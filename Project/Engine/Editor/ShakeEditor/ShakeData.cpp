#include "ShakeData.h"
#include "Frame/Frame.h"
#include "Shake.h"
#include <algorithm>
#include <imgui.h>
#include <Windows.h>

void ShakeData::Init(const std::string& shakeName) {
    globalParameter_ = GlobalParameter::GetInstance();
    groupName_       = shakeName;
    folderPath_      = "ShakeEditor";

    if (!globalParameter_->HasRegisters(shakeName)) {
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

void ShakeData::Update(const float& speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }

    // 実際のデルタタイムを計算
    float actualDeltaTime = 0.0f;
    switch (static_cast<TimeMode>(timeModeSelector_.GetTimeModeInt())) {
    case TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case TimeMode::DELTA_TIME_RATE:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    default:
        break;
    }

    // イージング更新
    timeEase_.SetMaxTime(maxTime_);
    timeEase_.SetType(static_cast<EasingType>(easeType_));
    timeEase_.Update(actualDeltaTime);

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

void ShakeData::Reset() {
    easedTime_          = startTime_;
    currentShakeOffset_ = Vector3::ZeroVector();
    timeEase_.Reset();
    playState_ = PlayState::STOPPED;
}

void ShakeData::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
    GetParams();
}

void ShakeData::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void ShakeData::RegisterParams() {
    globalParameter_->Regist(groupName_, "shakeLength", &shakeLength_);
    globalParameter_->Regist(groupName_, "maxTime", &maxTime_);
    globalParameter_->Regist(groupName_, "easeType", &easeType_);
    globalParameter_->Regist(groupName_, "shakeType", &shakeType_);
    globalParameter_->Regist(groupName_, "startTime", &startTime_);
    globalParameter_->Regist(groupName_, "axisFlag", &axisFlag_);
    timeModeSelector_.RegisterParam(groupName_, globalParameter_);
}

void ShakeData::GetParams() {
    shakeLength_ = globalParameter_->GetValue<float>(groupName_, "shakeLength");
    maxTime_     = globalParameter_->GetValue<float>(groupName_, "maxTime");
    easeType_    = globalParameter_->GetValue<int32_t>(groupName_, "easeType");
    shakeType_   = globalParameter_->GetValue<int32_t>(groupName_, "shakeType");
    startTime_   = globalParameter_->GetValue<float>(groupName_, "startTime");
    axisFlag_    = globalParameter_->GetValue<int32_t>(groupName_, "axisFlag");
    timeModeSelector_.GetParam(groupName_, globalParameter_);
}

void ShakeData::AdjustParam() {
#ifdef _DEBUG
    if (showControls_) {
        ImGui::SeparatorText(("Shake Editor: " + groupName_).c_str());
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
            if (x) {
                axisFlag_ |= AXIS_X;
            } else {
                axisFlag_ &= ~AXIS_X;
            }
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Y", &y)) {
            if (y) {
                axisFlag_ |= AXIS_Y;
            } else {
                axisFlag_ &= ~AXIS_Y;
            }
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Z", &z)) {
            if (z) {
                axisFlag_ |= AXIS_Z;
            } else {
                axisFlag_ &= ~AXIS_Z;
            }
        }

        ImGui::Separator();

        // パラメータ調整
        ImGui::DragFloat("Shake Length", &shakeLength_, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Max Time", &maxTime_, 0.01f, 0.1f, 10.0f);
        ImGui::DragFloat("Start Time", &startTime_, 0.01f, 0.1f, 1.0f);

        // シェイクタイプ
        const char* shakeTypeItems[] = {"Normal", "Wave"};
        ImGui::Combo("Shake Type", &shakeType_, shakeTypeItems, IM_ARRAYSIZE(shakeTypeItems));

        // イージングタイプ
        ImGuiEasingTypeSelector("Easing Type", easeType_);

        ImGui::Separator();

        // タイムモード設定
        timeModeSelector_.SelectTimeModeImGui("Time Mode");

        ImGui::PopID();
    }
#endif // _DEBUG
}

void ShakeData::InitParams() {
    // イージング設定
    timeEase_.SetAdaptValue(&easedTime_);
    timeEase_.SetStartValue(startTime_);
    timeEase_.SetEndValue(0.0f);

    timeEase_.SetOnFinishCallback([this]() {
        playState_          = PlayState::STOPPED;
        currentShakeOffset_ = Vector3::ZeroVector();
    });

    // 初期状態
    playState_          = PlayState::STOPPED;
    currentShakeOffset_ = {0.0f, 0.0f, 0.0f};
}