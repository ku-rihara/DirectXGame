#include "Easing.h"
#include "Function/GetFile.h"
#include<imGui.h>
#include <fstream>

template <typename T>
void Easing<T>::Reset() {
    isRunning_   = false;
    isFinished_  = false;
    currentTime_ = 0.0f;
}

// normal Easing
template <typename T>
void Easing<T>::SettingValue(const EasingType& easeType, const T& startValue, const T& endValue, const float& maxTime) {
    maxTime_    = maxTime;
    type_       = easeType;
    startValue_ = startValue;
    endValue_   = endValue;
}

// back Easing
template <typename T>
void Easing<T>::SettingValue(const EasingType& easeType, const T& startValue, const T& endValue, const float& maxTime, const float& backRate) {
    maxTime_    = maxTime;
    backRatio_  = backRate;
    type_       = easeType;
    startValue_ = startValue;
    endValue_   = endValue;
}

// amplitude Easing
template <typename T>
void Easing<T>::SettingValue(const EasingType& easeType, const T& startValue, const T& endValue, const float& maxTime, const float& amplitude, const float& period) {
    maxTime_    = maxTime;
    amplitude_  = amplitude;
    period_     = period;
    type_       = easeType;
    startValue_ = startValue;
    endValue_   = endValue;
}

template <typename T>
void Easing<T>::ApplyFromJson(const std::string& fileName) {
    // JSONファイルを読み込む
    std::ifstream ifs(fileName);
    if (!ifs.is_open()) {
      
        return;
    }

      nlohmann::json easingJson;
    ifs >> easingJson;

    EasingParameter<T> param;
    param.type       = static_cast<EasingType>(easingJson.at("type").get<int>());
    param.finishType = static_cast<EasingFinishValueType>(easingJson.at("finishType").get<int>());

    if constexpr (std::is_same_v<T, Vector2>) {
        const auto& sv   = easingJson.at("startValue");
        const auto& ev   = easingJson.at("endValue");
        param.startValue = Vector2{sv[0].get<float>(), sv[1].get<float>()};
        param.endValue   = Vector2{ev[0].get<float>(), ev[1].get<float>()};
    } else if constexpr (std::is_same_v<T, Vector3>) {
        const auto& sv   = easingJson.at("startValue");
        const auto& ev   = easingJson.at("endValue");
        param.startValue = Vector3{sv[0].get<float>(), sv[1].get<float>(), sv[2].get<float>()};
        param.endValue   = Vector3{ev[0].get<float>(), ev[1].get<float>(), ev[2].get<float>()};
    } else {
        param.startValue = easingJson.at("startValue").get<T>();
        param.endValue   = easingJson.at("endValue").get<T>();
    }

    param.maxTime   = easingJson.at("maxTime").get<float>();
    param.amplitude = easingJson.value("amplitude", 0.0f);
    param.period    = easingJson.value("period", 0.0f);
    param.backRatio = easingJson.value("backRatio", 0.0f);

    finishValueType_ = param.finishType;
    switch (param.type) {
    case EasingType::SquishyScaling:
        SettingValue(param.type, param.startValue, param.endValue, param.maxTime, param.amplitude, param.period);
        break;
    case EasingType::BackInSineZero:
    case EasingType::BackOutSineZero:
    case EasingType::BackInOutSineZero:
    case EasingType::BackInQuadZero:
    case EasingType::BackOutQuadZero:
    case EasingType::BackInOutQuadZero:
    case EasingType::BackInCubicZero:
    case EasingType::BackOutCubicZero:
    case EasingType::BackInOutCubicZero:
    case EasingType::BackInQuartZero:
    case EasingType::BackOutQuartZero:
    case EasingType::BackInOutQuartZero:
    case EasingType::BackInQuintZero:
    case EasingType::BackOutQuintZero:
    case EasingType::BackInOutQuintZero:
    case EasingType::BackInExpoZero:
    case EasingType::BackOutExpoZero:
    case EasingType::BackInOutExpoZero:
    case EasingType::BackInCircZero:
    case EasingType::BackOutCircZero:
    case EasingType::BackInOutCircZero:
        SettingValue(param.type, param.startValue, param.endValue, param.maxTime, param.backRatio);
        break;
    default:
        SettingValue(param.type, param.startValue, param.endValue, param.maxTime);
        break;
    }
}

template <typename T>
void Easing<T>::ApplyForImGui() {
    easingFiles_ = GetFileNamesForDyrectry(FilePath_);

    if (easingFiles_.empty()) {
        return;
    }

    // Cスタイル文字列に変換（ImGui::Combo用）
    std::vector<const char*> fileNamesCStr;
    for (const auto& name : easingFiles_) {
        fileNamesCStr.push_back(name.c_str());
    }

    // Combo UI表示
    if (ImGui::Combo("Easing Preset", &selectedFileIndex_, fileNamesCStr.data(), static_cast<int>(fileNamesCStr.size()))) {
        // 選択されたファイルのフルパスを作成
        const std::string selectedFile = FilePath_ + "/" + easingFiles_[selectedFileIndex_];

       
        ApplyFromJson(selectedFile);
    }
}
// 時間を進めて値を更新
template <typename T>
void Easing<T>::Update(float deltaTime) {
    if (/*!isRunning_ || */ isFinished_) {
        return;
    }

    currentTime_ += deltaTime;
    if (currentTime_ >= maxTime_) {
        FinishBehavior();
    }

    CalculateValue();
}

template <typename T>
void Easing<T>::CalculateValue() {

    switch (type_) {
    // --- 標準イージング ---
    case EasingType::InSine:
        *currentValue_ = EaseInSine(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutSine:
        *currentValue_ = EaseOutSine(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutSine:
        *currentValue_ = EaseInOutSine(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InQuint:
        *currentValue_ = EaseInQuint(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutQuint:
        *currentValue_ = EaseOutQuint(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuint:
        *currentValue_ = EaseInOutQuint(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InCirc:
        *currentValue_ = EaseInCirc(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutCirc:
        *currentValue_ = EaseOutCirc(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutCirc:
        *currentValue_ = EaseInOutCirc(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InExpo:
        *currentValue_ = EaseInExpo(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutExpo:
        *currentValue_ = EaseOutExpo(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutExpo:
        *currentValue_ = EaseInOutExpo(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InCubic:
        *currentValue_ = EaseInCubic(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutCubic:
        *currentValue_ = EaseOutCubic(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutCubic:
        *currentValue_ = EaseInOutCubic(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InQuad:
        *currentValue_ = EaseInQuad(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutQuad:
        *currentValue_ = EaseOutQuad(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuad:
        *currentValue_ = EaseInOutQuad(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InQuart:
        *currentValue_ = EaseInQuart(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutQuart:
        *currentValue_ = EaseOutQuart(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuart:
        /*    currentValue_ = EaseInOutQuart(startValue_, endValue_, currentTime_, maxTime_);*/
        break;
    case EasingType::InBack:
        *currentValue_ = EaseInBack(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutBack:
        *currentValue_ = EaseOutBack(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutBack:
        *currentValue_ = EaseInOutBack(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InBounce:
        *currentValue_ = EaseInBounce(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::OutBounce:
        *currentValue_ = EaseOutBounce(startValue_, endValue_, currentTime_, maxTime_);
        break;
    case EasingType::InOutBounce:
        *currentValue_ = EaseInOutBounce(startValue_, endValue_, currentTime_, maxTime_);
        break;

    //  特殊イージング
    case EasingType::SquishyScaling:
        *currentValue_ = EaseAmplitudeScale(startValue_, currentTime_, maxTime_, amplitude_, period_);
        break;

    //  Back
    case EasingType::BackInSineZero:
        *currentValue_ = Back::InSineZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackOutSineZero:
        *currentValue_ = Back::OutSineZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInOutSineZero:
        *currentValue_ = Back::InOutSineZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInQuadZero:
        *currentValue_ = Back::InQuadZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackOutQuadZero:
        *currentValue_ = Back::OutQuadZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInOutQuadZero:
        *currentValue_ = Back::InOutQuadZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInCubicZero:
        *currentValue_ = Back::InCubicZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackOutCubicZero:
        *currentValue_ = Back::OutCubicZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInOutCubicZero:
        *currentValue_ = Back::InOutCubicZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInQuartZero:
        *currentValue_ = Back::InQuartZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackOutQuartZero:
        *currentValue_ = Back::OutQuartZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInOutQuartZero:
        *currentValue_ = Back::InOutQuartZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInQuintZero:
        *currentValue_ = Back::InQuintZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackOutQuintZero:
        *currentValue_ = Back::OutQuintZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInOutQuintZero:
        *currentValue_ = Back::InOutQuintZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInExpoZero:
        *currentValue_ = Back::InExpoZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackOutExpoZero:
        *currentValue_ = Back::OutExpoZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInOutExpoZero:
        *currentValue_ = Back::InOutExpoZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInCircZero:
        *currentValue_ = Back::InCircZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackOutCircZero:
        *currentValue_ = Back::OutCircZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    case EasingType::BackInOutCircZero:
        *currentValue_ = Back::InOutCircZero(startValue_, endValue_, currentTime_, maxTime_, backRatio_);
        break;
    }
}
template <typename T>
void Easing<T>::Easing::FinishBehavior() {
    currentTime_ = maxTime_;
    isFinished_  = true;

    switch (finishValueType_) {
    case EasingFinishValueType::Start:
        *currentValue_ = startValue_;
        break;
    case EasingFinishValueType::End:
        *currentValue_ = endValue_;
        break;
    default:
        break;
    }
}

template class Easing<float>;
template class Easing<Vector2>;
template class Easing<Vector3>;