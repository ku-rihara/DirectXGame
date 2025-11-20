#include "ObjEaseAnimationKeyFrame.h"
#include "Frame/Frame.h"
#include "MathFunction.h"
#include <imgui.h>

void ObjEaseAnimationKeyFrame::Init(const std::string& animationName, const std::string& categoryName, const int32_t& keyNumber) {
    globalParameter_      = GlobalParameter::GetInstance();
    currentKeyFrameIndex_ = keyNumber;
    groupName_            = animationName + std::to_string(currentKeyFrameIndex_);
    folderPath_           = "ObjEaseAnimation/" + categoryName + "KeyFrames/" + animationName;

    // Scaleの初期値を1に設定
    transformParams_[static_cast<size_t>(TransformType::Scale)].endValue = Vector3::OneVector();

    // Rail初期化
    railPlayer_ = std::make_unique<RailPlayer>();
    railPlayer_->Init();

    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
    } else {
        LoadParams();
    }

    AdaptValueSetting();
    AdaptEaseParam();
}

void ObjEaseAnimationKeyFrame::Reset() {
    for (auto& param : transformParams_) {
        param.ease.Reset();
        param.currentOffset = param.ease.GetStartValue();
    }

    if (railPlayer_) {
        railPlayer_->Stop();
    }

    playState_ = PlayState::STOPPED;

    AdaptValueSetting();
    AdaptEaseParam();
}

void ObjEaseAnimationKeyFrame::LoadData() {
    globalParameter_->LoadFile(groupName_, folderPath_);
    globalParameter_->SyncParamForGroup(groupName_);
}

void ObjEaseAnimationKeyFrame::SaveData() {
    globalParameter_->SaveFile(groupName_, folderPath_);
}

void ObjEaseAnimationKeyFrame::Update(const float& speedRate) {
    float actualDeltaTime;

    // 時間モードに応じてデルタタイム取得
    switch (static_cast<TimeMode>(timeMode_)) {
    case TimeMode::DELTA_TIME:
        actualDeltaTime = Frame::DeltaTime() * speedRate;
        break;
    case TimeMode::DELTA_TIME_RATE:
    default:
        actualDeltaTime = Frame::DeltaTimeRate() * speedRate;
        break;
    }

    // 戻り動作更新
    if (playState_ == PlayState::RETURNING) {
        UpdateReturn(actualDeltaTime);
        return;
    }

    if (playState_ != PlayState::PLAYING) {
        return;
    }

    // 再生更新
    UpdatePlay(actualDeltaTime);
    CheckFinish();
}

void ObjEaseAnimationKeyFrame::UpdatePlay(const float& deltaTime) {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        // Activeでない場合のスキップ処理
        if (!param.isActive) {
            if (i == static_cast<size_t>(TransformType::Scale)) {
                param.currentOffset = Vector3::OneVector();
            }
            continue;
        }

        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            railPlayer_->Update(deltaTime);
            param.currentOffset = railPlayer_->GetCurrentPosition();
        } else {
            param.ease.Update(deltaTime);
        }
    }
}
void ObjEaseAnimationKeyFrame::UpdateReturn(const float& deltaTime) {

    bool allFinished = true;

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        if (!param.isActive || !param.isReturnToOrigin) {
            continue;
        }

        // Translation + Rail使用時の特殊処理
        if (i == static_cast<size_t>(TransformType::Translation)) {
            railPlayer_->Update(deltaTime);
            param.currentOffset = railPlayer_->GetCurrentPosition();
            if (railPlayer_->IsPlaying()) {
                allFinished = false;
            }
        } else {
            param.ease.Update(deltaTime);
            param.currentOffset = param.ease.GetValue();
            if (!param.ease.IsFinished()) {
                allFinished = false;
            }
        }
    }

    if (allFinished) {
        playState_ = PlayState::STOPPED;
    }
}

void ObjEaseAnimationKeyFrame::StartReturn() {
    playState_ = PlayState::RETURNING;

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        if (!param.isActive || !param.isReturnToOrigin) {
            continue;
        }

        // 戻り設定
        param.ease.SetIsStartEndReverse(true);
        param.ease.SetMaxTime(param.returnMaxTime);
        param.ease.SetType(static_cast<EasingType>(param.returnEaseType));
        param.ease.Reset();
    }
}

void ObjEaseAnimationKeyFrame::CheckFinish() {
    bool allFinished = true;

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const auto& param = transformParams_[i];

        if (!param.isActive)
            continue;

        // Translation + Rail使用時の特殊処理
        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            if (railPlayer_->IsPlaying()) {
                allFinished = false;
            }
        } else {
            if (!param.ease.IsFinished()) {
                allFinished = false;
            }
        }
    }

    if (allFinished) {
        // 戻り動作が必要かチェック
        bool needReturn = false;
        for (const auto& param : transformParams_) {
            if (param.isActive && param.isReturnToOrigin) {
                needReturn = true;
                break;
            }
        }

        if (needReturn) {
            StartReturn();
        } else {
            playState_ = PlayState::STOPPED;
        }
    }
}

void ObjEaseAnimationKeyFrame::RegisterParams() {
    globalParameter_->Regist(groupName_, "timePoint", &timePoint_);
    globalParameter_->Regist(groupName_, "timeMode", &timeMode_);

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const char* name = GetSRTName(static_cast<TransformType>(i));
        auto& param      = transformParams_[i];

        globalParameter_->Regist(groupName_, std::string(name) + "_IsActive", &param.isActive);
        globalParameter_->Regist(groupName_, std::string(name) + "_endValue", &param.endValue);
        globalParameter_->Regist(groupName_, std::string(name) + "_MaxTime", &param.maxTime);
        globalParameter_->Regist(groupName_, std::string(name) + "_EaseType", &param.easeType);
        globalParameter_->Regist(groupName_, std::string(name) + "_IsReturnToOrigin", &param.isReturnToOrigin);
        globalParameter_->Regist(groupName_, std::string(name) + "_ReturnEaseType", &param.returnEaseType);
        globalParameter_->Regist(groupName_, std::string(name) + "_ReturnMaxTime", &param.returnMaxTime);

        if (i == static_cast<size_t>(TransformType::Translation)) {
            globalParameter_->Regist(groupName_, std::string(name) + "_UseRail", &param.useRail);
            globalParameter_->Regist(groupName_, std::string(name) + "_RailFileName", &param.railFileName);
        }
    }
}

void ObjEaseAnimationKeyFrame::LoadParams() {
    timePoint_ = globalParameter_->GetValue<float>(groupName_, "timePoint");
    timeMode_  = globalParameter_->GetValue<int32_t>(groupName_, "timeMode");

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const char* name = GetSRTName(static_cast<TransformType>(i));
        auto& param      = transformParams_[i];

        param.isActive         = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_IsActive");
        param.endValue         = globalParameter_->GetValue<Vector3>(groupName_, std::string(name) + "_endValue");
        param.maxTime          = globalParameter_->GetValue<float>(groupName_, std::string(name) + "_MaxTime");
        param.easeType         = globalParameter_->GetValue<int32_t>(groupName_, std::string(name) + "_EaseType");
        param.isReturnToOrigin = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_IsReturnToOrigin");
        param.returnEaseType   = globalParameter_->GetValue<int32_t>(groupName_, std::string(name) + "_ReturnEaseType");
        param.returnMaxTime    = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_ReturnMaxTime");

        if (i == static_cast<size_t>(TransformType::Translation)) {
            param.useRail      = globalParameter_->GetValue<bool>(groupName_, std::string(name) + "_UseRail");
            param.railFileName = globalParameter_->GetValue<std::string>(groupName_, std::string(name) + "_RailFileName");
        }
    }
}

void ObjEaseAnimationKeyFrame::AdaptEaseParam() {
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        auto& param = transformParams_[i];

        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            continue;
        }

        param.ease.SetMaxTime(timePoint_);
        param.ease.SetEndValue(param.endValue);
        param.ease.SetType(static_cast<EasingType>(param.easeType));
    }
}

void ObjEaseAnimationKeyFrame::ImGuiTransformParam(const char* label, TransformParam& param, const TransformType& type) {
    ImGui::SeparatorText(label);
    ImGui::Checkbox((std::string(label) + " Active").c_str(), &param.isActive);
    ImGui::PushID(label);

    if (!param.isActive) {
        ImGui::PopID();
        return;
    }

    if (type == TransformType::Translation) {
        ImGui::Checkbox("Use Rail", &param.useRail);

        if (param.useRail) {
            std::string directory = "Resources/GlobalParameter/RailEditor/Dates";
            railFileSelector_.selector.SelectFile("Rail File", directory, param.railFileName, "", true);
            ImGui::PopID();
            return;
        }
    }

    if (type == TransformType::Rotation) {
        Vector3 endValueDeg = ToDegree(param.endValue);
        ImGui::DragFloat3("End Value (Degree)", &endValueDeg.x, 0.1f);
        param.endValue = ToRadian(endValueDeg);
    } else {
        ImGui::DragFloat3("End Value", &param.endValue.x, 0.01f);
    }

    ImGui::DragFloat("Max Time", &param.maxTime, 0.01f, 0.1f, 10.0f);
    ImGuiEasingTypeSelector("Easing Type", param.easeType);

    ImGui::PopID();
}

void ObjEaseAnimationKeyFrame::TimeModeSelector(const char* label, int32_t& target) {
    int mode = static_cast<int>(target);
    if (ImGui::Combo(label, &mode, TimeModeLabels.data(), static_cast<int>(TimeModeLabels.size()))) {
        target = mode;
    }
}

void ObjEaseAnimationKeyFrame::AdjustParam() {
#ifdef _DEBUG
    ImGui::SeparatorText(("KeyFrame: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    ImGui::DragFloat("Time Point", &timePoint_, 0.01f);
    TimeModeSelector("Time Mode", timeMode_);

    ImGui::Separator();

    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        ImGuiTransformParam(GetSRTName(static_cast<TransformType>(i)), transformParams_[i], static_cast<TransformType>(i));
    }

    AdaptEaseParam();

    ImGui::PopID();
#endif
}

void ObjEaseAnimationKeyFrame::AdaptValueSetting() {
    for (auto& param : transformParams_) {
        param.ease.SetAdaptValue(&param.currentOffset);
    }
}

void ObjEaseAnimationKeyFrame::SetStartValues(const Vector3& scale, const Vector3& rotation, const Vector3& translation) {
    transformParams_[static_cast<size_t>(TransformType::Scale)].ease.SetStartValue(scale);
    transformParams_[static_cast<size_t>(TransformType::Rotation)].ease.SetStartValue(rotation);
    transformParams_[static_cast<size_t>(TransformType::Translation)].ease.SetStartValue(translation);

    transformParams_[static_cast<size_t>(TransformType::Scale)].currentOffset       = scale;
    transformParams_[static_cast<size_t>(TransformType::Rotation)].currentOffset    = rotation;
    transformParams_[static_cast<size_t>(TransformType::Translation)].currentOffset = translation;
}

bool ObjEaseAnimationKeyFrame::IsFinished() const {

    // 戻りの場合は、戻り動作が完了しているかチェック
    if (playState_ == PlayState::RETURNING) {
        return playState_ == PlayState::STOPPED;
    }

    // PLAYING状態の場合は、再生が終わったか確認
    for (size_t i = 0; i < static_cast<size_t>(TransformType::Count); ++i) {
        const auto& param = transformParams_[i];

        if (!param.isActive) {
            continue;
        }

        if (i == static_cast<size_t>(TransformType::Translation) && param.useRail) {
            if (railPlayer_->IsPlaying()) {
                return false;
            }
        } else {
            if (!param.ease.IsFinished()) {
                return false;
            }
        }
    }
    return true;
}

bool ObjEaseAnimationKeyFrame::IsUsingRail() const {
    const auto& transParam = transformParams_[static_cast<size_t>(TransformType::Translation)];
    return transParam.useRail && transParam.isActive;
}

const char* ObjEaseAnimationKeyFrame::GetSRTName(const TransformType& type) const {
    switch (type) {
    case TransformType::Scale:
        return "Scale";
    case TransformType::Rotation:
        return "Rotation";
    case TransformType::Translation:
        return "Translation";
    default:
        return "Unknown";
    }
}