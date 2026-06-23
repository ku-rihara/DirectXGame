#include "Easing.h"

using namespace KetaEngine;
// Editor
#include "Editor/Easing/EasingFunction.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <string>
#include <type_traits>

template <typename T>
void Easing<T>::FilePathChangeForType() {
    if constexpr (std::is_same_v<T, float>) {
        filePathForType_ = "float";
    } else if constexpr (std::is_same_v<T, Vector2>) {
        filePathForType_ = "Vector2";
    } else if constexpr (std::is_same_v<T, Vector3>) {
        filePathForType_ = "Vector3";
    } else if constexpr (std::is_same_v<T, Vector4>) {
        filePathForType_ = "Vector4";
    }
}

template <typename T>
void Easing<T>::Init(const std::string& adaptFile) {
    if (adaptFile.empty())
        return;

    FilePathChangeForType();

    const auto dotPos        = adaptFile.rfind('.');
    const std::string stem   = (dotPos != std::string::npos) ? adaptFile.substr(0, dotPos) : adaptFile;
    const std::string folder = "EasingParameter/" + filePathForType_;

    // ローカル変数でエントリーを作成
    int32_t typeInt = 0, finishTypeInt = 0, returnTypeInt = 0;
    EasingParameter<T> param;

    auto* globalParameter = GlobalParameter::GetInstance();
    if (!globalParameter->HasGroup(stem)) {
        globalParameter->CreateGroup(stem);
    }

    globalParameter->Regist(stem, "type", &typeInt);
    globalParameter->Regist(stem, "finishType", &finishTypeInt);
    globalParameter->Regist(stem, "returnType", &returnTypeInt);
    globalParameter->Regist(stem, "maxTime", &param.maxTime);
    globalParameter->Regist(stem, "returnMaxTime", &param.returnMaxTime);
    globalParameter->Regist(stem, "amplitude", &param.amplitude);
    globalParameter->Regist(stem, "period", &param.period);
    globalParameter->Regist(stem, "finishOffsetTime", &param.finishOffsetTime);
    globalParameter->Regist(stem, "waitTime", &param.waitTimeMax);
    globalParameter->Regist(stem, "StartTimeOffset", &param.startTimeOffset);
    globalParameter->Regist(stem, "startValue", &param.startValue);
    globalParameter->Regist(stem, "endValue", &param.endValue);

    globalParameter->LoadFile(stem, folder);
    globalParameter->SyncParamForGroup(stem);
    globalParameter->ClearRegistersForGroup(stem); 

    param.type       = static_cast<EasingType>(typeInt);
    param.finishType = static_cast<EasingFinishValueType>(finishTypeInt);
    param.returnType = static_cast<EasingType>(returnTypeInt);

    SettingValue(param);
}

template <typename T>
void Easing<T>::Reset() {
    isFinished_             = false;
    currentTime_            = 0.0f;
    waitTime_               = 0.0f;
    currentStartTimeOffset_ = 0.0f;
    CalculateValue();
}

template <typename T>
void Easing<T>::SettingValue(const EasingParameter<T>& easingParam) {
    type_            = easingParam.type;
    finishValueType_ = easingParam.finishType;

    startValue_ = easingParam.startValue;
    endValue_   = easingParam.endValue;
    amplitude_  = easingParam.amplitude;
    period_     = easingParam.period;

    returnType_    = easingParam.returnType;
    returnMaxTime_ = easingParam.returnMaxTime;

    if (easingParam.returnMaxTime > 0.0f) {
        forwardMaxTime_  = easingParam.maxTime;
        maxTime_         = easingParam.maxTime + easingParam.returnMaxTime;
        finishValueType_ = EasingFinishValueType::Start;
    } else {
        forwardMaxTime_  = easingParam.maxTime;
        maxTime_         = easingParam.maxTime;
        finishValueType_ = EasingFinishValueType::End;
    }

    waitTimeMax_      = easingParam.waitTimeMax;
    finishTimeOffset_ = easingParam.finishOffsetTime;
    startTimeOffset_  = easingParam.startTimeOffset;
}

// 時間を進めて値を更新
template <typename T>
void Easing<T>::Update(float deltaTime) {
    currentStartTimeOffset_ += deltaTime;

    if (!IsEasingStarted()) {
        return;
    }

    if (!isFinished_) {
        currentTime_ += deltaTime;
    }

    isPlaying_ = true;
    CalculateValue();

    if (currentTime_ < maxTime_ - finishTimeOffset_) {
        return;
    }

    isPlaying_ = false;
    FinishBehavior();

    if (onFinishCallback_) {
        onFinishCallback_();
    }

    waitTime_ += deltaTime;

    if (waitTime_ < waitTimeMax_) {
        return;
    }

    if (onWaitEndCallback_) {
        onWaitEndCallback_();
    }
}

template <typename T>
void Easing<T>::ResetStartValue() {
    currentTime_ = 0.0f;
    CalculateValue();
}

template <typename T>
void Easing<T>::CalculateValue() {
    T startValue = {};
    T endValue   = {};
    if (isStartEndReverse_) {
        startValue = endValue_ + baseValue_;
        endValue   = startValue_ + baseValue_;
    } else {
        startValue = startValue_ + baseValue_;
        endValue   = endValue_ + baseValue_;
    }

    if (!adaptTarget_) {
        return;
    }

    if (maxTime_ <= 0.0f) {
        *adaptTarget_ = endValue;
        return;
    }

    float savedTime      = currentTime_;
    float savedMax       = maxTime_;
    EasingType savedType = type_;

    if (returnMaxTime_ > 0.0f) {
        if (savedTime <= forwardMaxTime_) {
            currentTime_ = savedTime;
            maxTime_     = forwardMaxTime_;
        } else {
            std::swap(startValue, endValue);
            currentTime_ = savedTime - forwardMaxTime_;
            maxTime_     = returnMaxTime_;
            type_        = returnType_;
        }
    }

    switch (type_) {
    case EasingType::InSine:
        *adaptTarget_ = EaseInSine(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutSine:
        *adaptTarget_ = EaseOutSine(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutSine:
        *adaptTarget_ = EaseInOutSine(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InQuint:
        *adaptTarget_ = EaseInQuint(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutQuint:
        *adaptTarget_ = EaseOutQuint(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuint:
        *adaptTarget_ = EaseInOutQuint(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InCirc:
        *adaptTarget_ = EaseInCirc(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutCirc:
        *adaptTarget_ = EaseOutCirc(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutCirc:
        *adaptTarget_ = EaseInOutCirc(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InExpo:
        *adaptTarget_ = EaseInExpo(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutExpo:
        *adaptTarget_ = EaseOutExpo(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutExpo:
        *adaptTarget_ = EaseInOutExpo(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InCubic:
        *adaptTarget_ = EaseInCubic(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutCubic:
        *adaptTarget_ = EaseOutCubic(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutCubic:
        *adaptTarget_ = EaseInOutCubic(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InQuad:
        *adaptTarget_ = EaseInQuad(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutQuad:
        *adaptTarget_ = EaseOutQuad(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuad:
        *adaptTarget_ = EaseInOutQuad(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InQuart:
        *adaptTarget_ = EaseInQuart(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutQuart:
        *adaptTarget_ = EaseOutQuart(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuart:
        break;
    case EasingType::InBack:
        *adaptTarget_ = EaseInBack(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutBack:
        *adaptTarget_ = EaseOutBack(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutBack:
        *adaptTarget_ = EaseInOutBack(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InBounce:
        *adaptTarget_ = EaseInBounce(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutBounce:
        *adaptTarget_ = EaseOutBounce(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutBounce:
        *adaptTarget_ = EaseInOutBounce(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::SquishyScaling:
        *adaptTarget_ = EaseAmplitudeScale(startValue, currentTime_, maxTime_, amplitude_, period_);
        break;
    }

    if (returnMaxTime_ > 0.0f) {
        currentTime_ = savedTime;
        maxTime_     = savedMax;
        type_        = savedType;
    }
}

template <typename T>
void Easing<T>::Easing::FinishBehavior() {
    currentTime_ = maxTime_;
    isFinished_  = true;
    isPlaying_   = false;

    if (!adaptTarget_) {
        return;
    }

    switch (finishValueType_) {
    case EasingFinishValueType::Start:
        *adaptTarget_ = isStartEndReverse_ ? endValue_ + baseValue_ : startValue_ + baseValue_;
        break;
    case EasingFinishValueType::End:
        *adaptTarget_ = isStartEndReverse_ ? startValue_ + baseValue_ : endValue_ + baseValue_;
        break;
    default:
        break;
    }
}

template <typename T>
void Easing<T>::Easing::SetAdaptValue(T* value) {
    adaptTarget_ = value;
}

template <typename T>
void Easing<T>::Easing::SetCurrentOffset(const T& value) {
    *adaptTarget_ = value;
}

template <typename T>
bool Easing<T>::IsEasingStarted() const {
    return currentStartTimeOffset_ >= startTimeOffset_;
}

template class Easing<float>;
template class Easing<Vector2>;
template class Easing<Vector3>;
template class Easing<Vector4>;
