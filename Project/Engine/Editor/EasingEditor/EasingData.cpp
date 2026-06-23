#include "EasingData.h"

using namespace KetaEngine;
#include "Editor/Easing/EasingCreator/EasingParameterData.h"
#include "Frame/Frame.h"
#include "Vector2.h"
#include "Vector3.h"
#include <imgui.h>
#include <type_traits>

template <typename T>
std::string EasingData<T>::GetStaticFolderName() {
    if constexpr (std::is_same_v<T, float>) {
        return "EasingEditor/Float/";
    } else if constexpr (std::is_same_v<T, Vector2>) {
        return "EasingEditor/Vec2/";
    } else if constexpr (std::is_same_v<T, Vector3>) {
        return "EasingEditor/Vec3/";
    }
}

template <typename T>
void EasingData<T>::Init(const std::string& name, const std::string& categoryName) {
    BaseEffectData::Init(name, categoryName);
    folderPath_ = GetStaticFolderName() + categoryName_ + "/Dates";

    if (!globalParameter_->HasRegisters(groupName_)) {
        globalParameter_->CreateGroup(groupName_);
        RegisterParams();
        globalParameter_->SyncParamForGroup(groupName_);
        ownsRegistration_ = true;
    } else {
        GetParams();
        ownsRegistration_ = false;
    }

    InitParams();
}

template <typename T>
void EasingData<T>::RegisterParams() {
    globalParameter_->Regist(groupName_, "type",            &typeInt_);
    globalParameter_->Regist(groupName_, "finishType",      &finishTypeInt_);
    globalParameter_->Regist(groupName_, "returnType",      &returnTypeInt_);
    globalParameter_->Regist(groupName_, "startValue",      &param_.startValue);
    globalParameter_->Regist(groupName_, "endValue",        &param_.endValue);
    globalParameter_->Regist(groupName_, "maxTime",         &param_.maxTime);
    globalParameter_->Regist(groupName_, "returnMaxTime",   &param_.returnMaxTime);
    globalParameter_->Regist(groupName_, "amplitude",       &param_.amplitude);
    globalParameter_->Regist(groupName_, "period",          &param_.period);
    globalParameter_->Regist(groupName_, "waitTimeMax",     &param_.waitTimeMax);
    globalParameter_->Regist(groupName_, "finishOffsetTime",&param_.finishOffsetTime);
    globalParameter_->Regist(groupName_, "startTimeOffset", &param_.startTimeOffset);
}

template <typename T>
void EasingData<T>::GetParams() {
    typeInt_       = globalParameter_->GetValue<int32_t>(groupName_, "type");
    finishTypeInt_ = globalParameter_->GetValue<int32_t>(groupName_, "finishType");
    returnTypeInt_ = globalParameter_->GetValue<int32_t>(groupName_, "returnType");

    param_.startValue       = globalParameter_->GetValue<T>(groupName_, "startValue");
    param_.endValue         = globalParameter_->GetValue<T>(groupName_, "endValue");
    param_.maxTime          = globalParameter_->GetValue<float>(groupName_, "maxTime");
    param_.returnMaxTime    = globalParameter_->GetValue<float>(groupName_, "returnMaxTime");
    param_.amplitude        = globalParameter_->GetValue<float>(groupName_, "amplitude");
    param_.period           = globalParameter_->GetValue<float>(groupName_, "period");
    param_.waitTimeMax      = globalParameter_->GetValue<float>(groupName_, "waitTimeMax");
    param_.finishOffsetTime = globalParameter_->GetValue<float>(groupName_, "finishOffsetTime");
    param_.startTimeOffset  = globalParameter_->GetValue<float>(groupName_, "startTimeOffset");

    param_.type       = static_cast<EasingType>(typeInt_);
    param_.finishType = static_cast<EasingFinishValueType>(finishTypeInt_);
    param_.returnType = static_cast<EasingType>(returnTypeInt_);
}

template <typename T>
void EasingData<T>::InitParams() {
    previewEasing_.SetAdaptValue(&previewValue_);
    previewEasing_.SetOnFinishCallback([this]() {
        playState_ = PlayState::STOPPED;
    });
    playState_ = PlayState::STOPPED;
}

template <typename T>
void EasingData<T>::Update(float speedRate) {
    if (playState_ != PlayState::PLAYING) {
        return;
    }
    previewEasing_.Update(Frame::DeltaTime() * speedRate);
}

template <typename T>
void EasingData<T>::Play() {
    Reset();
    param_.type       = static_cast<EasingType>(typeInt_);
    param_.finishType = static_cast<EasingFinishValueType>(finishTypeInt_);
    param_.returnType = static_cast<EasingType>(returnTypeInt_);
    previewEasing_.SettingValue(param_);
    previewEasing_.Reset();
    playState_ = PlayState::PLAYING;
}

template <typename T>
void EasingData<T>::Reset() {
    previewEasing_.Reset();
    playState_ = PlayState::STOPPED;
}

template <typename T>
void EasingData<T>::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    ImGui::SeparatorText(("Easing: " + groupName_).c_str());
    ImGui::PushID(groupName_.c_str());

    DrawPlayButton();

    ImGui::Text("State: %s", playState_ == PlayState::PLAYING ? "PLAYING" : "STOPPED");

    ImGui::Separator();

    ImGuiEasingTypeSelector("Easing Type", typeInt_);
    param_.type = static_cast<EasingType>(typeInt_);

    int finishType = finishTypeInt_;
    if (ImGui::Combo("Finish Type", &finishType, FinishTypeLabels.data(), static_cast<int>(FinishTypeLabels.size()))) {
        finishTypeInt_    = finishType;
        param_.finishType = static_cast<EasingFinishValueType>(finishTypeInt_);
    }

    ImGui::Separator();

    if constexpr (std::is_same_v<T, float>) {
        ImGui::DragFloat("Start Value", &param_.startValue, 0.01f);
        ImGui::DragFloat("End Value",   &param_.endValue,   0.01f);
    } else if constexpr (std::is_same_v<T, Vector2>) {
        ImGui::DragFloat2("Start Value", &param_.startValue.x, 0.01f);
        ImGui::DragFloat2("End Value",   &param_.endValue.x,   0.01f);
    } else if constexpr (std::is_same_v<T, Vector3>) {
        ImGui::DragFloat3("Start Value", &param_.startValue.x, 0.01f);
        ImGui::DragFloat3("End Value",   &param_.endValue.x,   0.01f);
    }

    ImGui::DragFloat("Max Time",      &param_.maxTime,          0.01f, 0.0f, 60.0f);
    ImGui::DragFloat("Amplitude",     &param_.amplitude,        0.01f);
    ImGui::DragFloat("Period",        &param_.period,           0.01f);
    ImGui::DragFloat("Wait Time",     &param_.waitTimeMax,      0.01f, 0.0f, 60.0f);
    ImGui::DragFloat("Start Offset",  &param_.startTimeOffset,  0.01f);
    ImGui::DragFloat("Finish Offset", &param_.finishOffsetTime, 0.01f);

    if (ImGui::TreeNode("Return Phase")) {
        ImGuiEasingTypeSelector("Return Type", returnTypeInt_);
        param_.returnType = static_cast<EasingType>(returnTypeInt_);
        ImGui::DragFloat("Return Time", &param_.returnMaxTime, 0.01f, 0.0f, 60.0f);
        ImGui::TreePop();
    }

    ImGui::PopID();
#endif
}

template class EasingData<float>;
template class EasingData<Vector2>;
template class EasingData<Vector3>;
