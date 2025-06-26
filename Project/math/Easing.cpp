#include "Easing.h"
#include "Function/GetFile.h"
#include "utility/EasingCreator/EasingCreator.h"
#include <fstream>
#include <imGui.h>
#include <Windows.h>

template <typename T>
void Easing<T>::Init(const std::string& name) {
    FilePathChangeForType();
    easingFiles_ = GetFileNamesForDyrectry(FilePath_ + filePathForType_);
    easingName_  = name;
    LoadAndApplyFromSavedJson();
}

template <typename T>
void Easing<T>::Reset() {

    isFinished_  = false;
    currentTime_ = 0.0f;
    waitTime_    = 0.0f;
}

template <typename T>
void Easing<T>::SettingValue(const EasingParameter<T>& easingParam) {

    type_                 = easingParam.type;
    adaptFloatAxisType_   = easingParam.adaptFloatAxisType;
    adaptVector2AxisType_ = easingParam.adaptVec2AxisType;
    finishValueType_      = easingParam.finishType;

    maxTime_    = easingParam.maxTime;
    startValue_ = easingParam.startValue;
    endValue_   = easingParam.endValue;
    amplitude_  = easingParam.amplitude;
    period_     = easingParam.period;
    backRatio_  = easingParam.backRatio;

    waitTimeMax_      = easingParam.waitTimeMax;
    finishTimeOffset_ = easingParam.finishOffsetTime;
}

template <typename T>
void Easing<T>::SaveAppliedJsonFileName() {
    if (currentAppliedFileName_.empty()) {
        return;
    }

    FilePathChangeForType();

    std::string savePath = FilePath_ + easingName_ + filePathForType_ + ".json";

    nlohmann::json saveJson;
    saveJson["appliedFileName"] = currentAppliedFileName_;
    saveJson["type"]            = filePathForType_;

    std::ofstream ofs(savePath);
    if (ofs.is_open()) {
        ofs << saveJson.dump(4);
        ofs.close();
    }
}

template <typename T>
void Easing<T>::LoadAndApplyFromSavedJson() {
    FilePathChangeForType();

    std::string savePath = FilePath_ + easingName_ + filePathForType_ + ".json";

    std::ifstream ifs(savePath);
    if (!ifs.is_open()) {
        return;
    }

    nlohmann::json saveJson;
    ifs >> saveJson;

    if (saveJson.contains("appliedFileName")) {
        std::string savedFileName = saveJson["appliedFileName"].get<std::string>();
        if (!savedFileName.empty()) {
            // 保存されていたJSONファイルを適用
            ApplyFromJson(savedFileName);
        }
    }
}

template <typename T>
void Easing<T>::ApplyFromJson(const std::string& fileName) {
    FilePathChangeForType();

    currentSelectedFileName_ = FilePath_ + filePathForType_ + "/" + fileName;

    std::ifstream ifs(currentSelectedFileName_);
    if (!ifs.is_open()) {
        return;
    }

    nlohmann::json easingJson;
    ifs >> easingJson;

    if (easingJson.empty()) {
        return;
    }

    // 以前の値を保存
    oldTypeVector2_ = adaptVector2AxisType_;
    oldTypeFloat_   = adaptFloatAxisType_;

    // JSONの内容を確認
    const auto& inner = easingJson.begin().value();

    // jsonからEasingParameterを取得
    EasingParameter<T> param;
    param.type       = static_cast<EasingType>(inner.at("type").get<int>());
    param.finishType = static_cast<EasingFinishValueType>(inner.at("finishType").get<int>());

    if constexpr (std::is_same_v<T, Vector3>) {
        const auto& sv   = inner.at("startValue");
        const auto& ev   = inner.at("endValue");
        param.startValue = Vector3{sv[0].get<float>(), sv[1].get<float>(), sv[2].get<float>()};
        param.endValue   = Vector3{ev[0].get<float>(), ev[1].get<float>(), ev[2].get<float>()};

    } else if constexpr (std::is_same_v<T, Vector2>) {
        const auto& sv          = inner.at("startValue");
        const auto& ev          = inner.at("endValue");
        param.startValue        = Vector2{sv[0].get<float>(), sv[1].get<float>()};
        param.endValue          = Vector2{ev[0].get<float>(), ev[1].get<float>()};
        param.adaptVec2AxisType = static_cast<AdaptVector2AxisType>(inner.at("adaptVec2AxisType").get<int>());

    } else if constexpr (std::is_same_v<T, float>) {
        param.startValue         = inner.at("startValue").get<float>();
        param.endValue           = inner.at("endValue").get<float>();
        param.adaptFloatAxisType = static_cast<AdaptFloatAxisType>(inner.at("adaptFloatAxisType").get<int>());
    }

    param.maxTime   = inner.at("maxTime").get<float>();
    param.amplitude = inner.value("amplitude", 0.0f);
    param.period    = inner.value("period", 0.0f);
    param.backRatio = inner.value("backRatio", 0.0f);

    param.finishOffsetTime = inner.value("finishOffsetTime", 0.0f);
    param.waitTimeMax      = inner.value("waitTime", 0.0f);

    // paramの値をセット
    SettingValue(param);

    // 適用されたファイル名を更新
    currentAppliedFileName_ = fileName;
    auto it                 = std::find(easingFiles_.begin(), easingFiles_.end(), fileName.substr(0, fileName.size() - 5)); //-5は".json"の長さ
    if (it != easingFiles_.end()) {
        selectedFileIndex_ = static_cast<int>(std::distance(easingFiles_.begin(), it));
    }

    // 軸が変わった場合、軸を変えた上で値を更新
    ChangeAdaptAxis();
}

template <typename T>
void Easing<T>::ChangeAdaptAxis() {

    if constexpr (std::is_same_v<T, Vector2>) {

        // 適応するVector2の軸が変わった場合、値を更新
        if (oldTypeVector2_ != adaptVector2AxisType_ && adaptTargetVec3_) {
            SetAdaptValue<Vector2>(adaptTargetVec3_);
        }

    } else if constexpr (std::is_same_v<T, float>) {

        // 適応するfloatの軸が変わった場合、値を更新
        if (oldTypeFloat_ != adaptFloatAxisType_) {

            if (adaptTargetVec3_) {
                SetAdaptValue<float>(adaptTargetVec3_);
            }
            if (adaptTargetVec2_) {
                SetAdaptValue<float>(adaptTargetVec2_);
            }
        }
    }
}

template <typename T>
void Easing<T>::ApplyForImGui() {
    FilePathChangeForType();

    easingFiles_ = GetFileNamesForDyrectry(FilePath_ + filePathForType_);

    if (easingFiles_.empty()) {
        return;
    }

    std::vector<const char*> fileNamesCStr;
    for (const auto& name : easingFiles_) {
        fileNamesCStr.push_back(name.c_str());
    }
#ifdef _DEBUG

    // 現在適用されているファイル名を表示
    if (!currentAppliedFileName_.empty()) {
        ImGui::Text("Currently Applied: %s", currentAppliedFileName_.c_str());
    }

    // Combo UI表示
    if (ImGui::Combo("Easing Preset", &selectedFileIndex_, fileNamesCStr.data(), static_cast<int>(fileNamesCStr.size()))) {
        // Comboで選択が変更された時
        const std::string selectedFile = easingFiles_[selectedFileIndex_] + ".json";

        // 同じファイルが既に適用されている場合はスキップ
        if (currentAppliedFileName_ == selectedFile) {
            return;
        }
        // 選択されたファイルを適用
        ApplyFromJson(selectedFile);
    }

    // ロード
    if (ImGui::Button("Load")) {
        LoadAndApplyFromSavedJson();
    }

    // 保存
    if (ImGui::Button("Save")) {
        SaveAppliedJsonFileName();
        std::string filename = "EasingAdaptFile";
        std::string message  = std::format("{}.json saved.", filename);
        MessageBoxA(nullptr, message.c_str(), "Easing", 0);
    }
#endif // _DEBUG
}

// 時間を進めて値を更新
template <typename T>
void Easing<T>::Update(float deltaTime) {
    if (!isFinished_) {
        currentTime_ += deltaTime;
    }

    CalculateValue();

    if (vector2Proxy_) {
        vector2Proxy_->Apply();
    }

    //  終了時間を過ぎたら終了処理
    if (currentTime_ < maxTime_ - finishTimeOffset_) {
        return;
    }

    FinishBehavior();

    if (onFinishCallback_) { // Easing終了時のコールバック
        onFinishCallback_();
    }

    // 待機時間の加算
    waitTime_ += deltaTime;

    if (waitTime_ < waitTimeMax_) {
        return;
    }

    if (onWaitEndCallback_) { // 待機終了時のコールバック
        onWaitEndCallback_();
    }
}

template <typename T>
void Easing<T>::FilePathChangeForType() {
    if constexpr (std::is_same_v<T, float>) {
        filePathForType_ = "float";

    } else if constexpr (std::is_same_v<T, Vector2>) {
        filePathForType_ = "Vector2";

    } else if constexpr (std::is_same_v<T, Vector3>) {
        filePathForType_ = "Vector3";
    }
}

template <typename T>
void Easing<T>::CalculateValue() {

    switch (type_) {

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
template <typename T>
void Easing<T>::Easing::SetAdaptValue(T* value) {
    currentValue_ = value;
}

template <>
template <>
void Easing<float>::SetAdaptValue<float>(Vector2* value) {
    adaptTargetVec2_ = value;
    switch (adaptFloatAxisType_) {
    case AdaptFloatAxisType::X:
        currentValue_ = &value->x;
        break;
    case AdaptFloatAxisType::Y:
        currentValue_ = &value->y;
        break;
    default:
        currentValue_ = &value->x;
        break;
    }
}

template <>
template <>
void Easing<float>::SetAdaptValue<float>(Vector3* value) {
    adaptTargetVec3_ = value;
    switch (adaptFloatAxisType_) {
    case AdaptFloatAxisType::X:
        currentValue_ = &value->x;
        break;
    case AdaptFloatAxisType::Y:
        currentValue_ = &value->y;
        break;
    case AdaptFloatAxisType::Z:
        currentValue_ = &value->z;
        break;
    default:
        currentValue_ = &value->x;
        break;
    }
}

template <>
template <>
void Easing<Vector2>::SetAdaptValue<Vector2>(Vector3* value) {
    adaptTargetVec3_ = value;

    switch (adaptVector2AxisType_) {
    case AdaptVector2AxisType::XY:
        vector2Proxy_ = std::make_unique<XYProxy>(value);

        break;
    case AdaptVector2AxisType::XZ:
        vector2Proxy_ = std::make_unique<XZProxy>(value);

        break;
    case AdaptVector2AxisType::YZ:
        vector2Proxy_ = std::make_unique<YZProxy>(value);

        break;
    default:
        vector2Proxy_ = std::make_unique<XYProxy>(value);

        break;
    }
    currentValue_ = &vector2Proxy_->Get();
}

template <typename T>
void Easing<T>::Easing::SetCurrentValue(const T& value) {
    *currentValue_ = value;
}


template class Easing<float>;
template class Easing<Vector2>;
template class Easing<Vector3>;