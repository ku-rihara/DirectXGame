#include "Easing.h"

using namespace KetaEngine;
#include "Easing/EasingFunction.h"
#include "Function/GetFile.h"
#include "Easing/EasingCreator/EasingCreator.h"
#include <fstream>
#include <imGui.h>
#include <Windows.h>
#include <unordered_map>

// ファイルI/Oキャッシュ
static std::unordered_map<std::string, nlohmann::json> s_easingJsonCache;
static std::unordered_map<std::string, std::vector<std::string>> s_easingDirCache;

template <typename T>
void Easing<T>::Init(const std::string& adaptFile) {

    // 初期化、ファイル読み込み
    FilePathChangeForType();
    const std::string dirKey = FilePath_ + filePathForType_;
    auto dirIt = s_easingDirCache.find(dirKey);
    if (dirIt != s_easingDirCache.end()) {
        easingFiles_ = dirIt->second;
    } else {
        easingFiles_ = GetFileNamesForDirectory(dirKey);
        s_easingDirCache[dirKey] = easingFiles_;
    }
  
    if (!adaptFile.empty()) {
        ApplyFromJson(adaptFile);
    }
}

template <typename T>
void Easing<T>::Reset() {

    // 　パラメータリセット
    isFinished_             = false;
    currentTime_            = 0.0f;
    waitTime_               = 0.0f;
    currentStartTimeOffset_ = 0.0f;
    CalculateValue();
}

template <typename T>
void Easing<T>::SettingValue(const EasingParameter<T>& easingParam) {

    // イージングのパラメータをセット

    type_                 = easingParam.type;
    adaptFloatAxisType_   = easingParam.adaptFloatAxisType;
    adaptVector2AxisType_ = easingParam.adaptVec2AxisType;
    finishValueType_      = easingParam.finishType;

    startValue_ = easingParam.startValue;
    endValue_   = easingParam.endValue;
    amplitude_  = easingParam.amplitude;
    period_     = easingParam.period;

    returnType_    = easingParam.returnType;
    returnMaxTime_ = easingParam.returnMaxTime;

    if (easingParam.returnMaxTime > 0.0f) {
        // 前進＋後退の2フェーズ: maxTime_ は合計時間
        forwardMaxTime_ = easingParam.maxTime;
        maxTime_        = easingParam.maxTime + easingParam.returnMaxTime;
        finishValueType_ = EasingFinishValueType::Start; // 終了時は起点に戻る
    } else {
        forwardMaxTime_  = easingParam.maxTime;
        maxTime_         = easingParam.maxTime;
        finishValueType_ = EasingFinishValueType::End;
    }

    waitTimeMax_      = easingParam.waitTimeMax;
    finishTimeOffset_ = easingParam.finishOffsetTime;
    startTimeOffset_  = easingParam.startTimeOffset;
}


template <typename T>
void Easing<T>::ApplyFromJson(const std::string& fileName) {
    FilePathChangeForType();

    currentSelectedFileName_ = FilePath_ + filePathForType_ + "/" + fileName;

    nlohmann::json easingJson;
    auto cacheIt = s_easingJsonCache.find(currentSelectedFileName_);
    if (cacheIt != s_easingJsonCache.end()) {
        easingJson = cacheIt->second;
    } else {
        std::ifstream ifs(currentSelectedFileName_);
        if (!ifs.is_open()) {
            return;
        }
        ifs >> easingJson;
        if (easingJson.empty()) {
            return;
        }
        s_easingJsonCache[currentSelectedFileName_] = easingJson;
    }

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

    if constexpr (std::is_same_v<T, Vector4>) {
        const auto& sv   = inner.at("startValue");
        const auto& ev   = inner.at("endValue");
        param.startValue = Vector4{sv[0].get<float>(), sv[1].get<float>(), sv[2].get<float>(), sv[3].get<float>()};
        param.endValue   = Vector4{ev[0].get<float>(), ev[1].get<float>(), ev[2].get<float>(), ev[3].get<float>()};

    } else if constexpr (std::is_same_v<T, Vector3>) {
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

    param.maxTime       = inner.at("maxTime").get<float>();
    param.returnMaxTime = inner.value("returnMaxTime", 0.0f);
    param.returnType    = static_cast<EasingType>(inner.value("returnType", 0));
    param.amplitude     = inner.value("amplitude", 0.0f);
    param.period        = inner.value("period", 0.0f);

    param.finishOffsetTime = inner.value("finishOffsetTime", 0.0f);
    param.waitTimeMax      = inner.value("waitTime", 0.0f);

    if (inner.contains("StartTimeOffset")) {
        param.startTimeOffset = inner.value("StartTimeOffset", 0.0f);
    }

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

    easingFiles_ = GetFileNamesForDirectory(FilePath_ + filePathForType_);

    if (easingFiles_.empty()) {
        return;
    }

    std::vector<const char*> fileNamesCStr;
    for (const auto& name : easingFiles_) {
        fileNamesCStr.push_back(name.c_str());
    }
#if defined(_DEBUG) || defined(DEVELOPMENT)

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

#endif // _DEBUG
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

    if (vector2Proxy_) {
        vector2Proxy_->Apply();
    }

    //  終了時間を過ぎたら終了処理
    if (currentTime_ < maxTime_ - finishTimeOffset_) {
        return; // まだ再生中
    }

    isPlaying_ = false;
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
void Easing<T>::ResetStartValue() {
    currentTime_ = 0.0f;
    CalculateValue();
}

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

    // currentOffsetのイージング計算
    if (!currentOffset_) {
        return;
    }

    // maxTime_ が 0 の場合は終端値を即セットして終了
    if (maxTime_ <= 0.0f) {
        *currentOffset_ = endValue;
        return;
    }

    float savedTime = currentTime_;
    float savedMax  = maxTime_;
    EasingType savedType = type_;

    if (returnMaxTime_ > 0.0f) {
        if (savedTime <= forwardMaxTime_) {
            // 前進フェーズ
            currentTime_ = savedTime;
            maxTime_     = forwardMaxTime_;
        } else {
            // 後退フェーズ
            std::swap(startValue, endValue);
            currentTime_ = savedTime - forwardMaxTime_;
            maxTime_     = returnMaxTime_;
            type_        = returnType_;
        }
    }

    switch (type_) {

    case EasingType::InSine:
        *currentOffset_ = EaseInSine(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutSine:
        *currentOffset_ = EaseOutSine(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutSine:
        *currentOffset_ = EaseInOutSine(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InQuint:
        *currentOffset_ = EaseInQuint(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutQuint:
        *currentOffset_ = EaseOutQuint(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuint:
        *currentOffset_ = EaseInOutQuint(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InCirc:
        *currentOffset_ = EaseInCirc(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutCirc:
        *currentOffset_ = EaseOutCirc(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutCirc:
        *currentOffset_ = EaseInOutCirc(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InExpo:
        *currentOffset_ = EaseInExpo(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutExpo:
        *currentOffset_ = EaseOutExpo(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutExpo:
        *currentOffset_ = EaseInOutExpo(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InCubic:
        *currentOffset_ = EaseInCubic(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutCubic:
        *currentOffset_ = EaseOutCubic(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutCubic:
        *currentOffset_ = EaseInOutCubic(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InQuad:
        *currentOffset_ = EaseInQuad(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutQuad:
        *currentOffset_ = EaseOutQuad(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuad:
        *currentOffset_ = EaseInOutQuad(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InQuart:
        *currentOffset_ = EaseInQuart(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutQuart:
        *currentOffset_ = EaseOutQuart(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutQuart:
        /*    currentOffset_ = EaseInOutQuart(startValue, endValue, currentTime_, maxTime_);*/
        break;
    case EasingType::InBack:
        *currentOffset_ = EaseInBack(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutBack:
        *currentOffset_ = EaseOutBack(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutBack:
        *currentOffset_ = EaseInOutBack(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InBounce:
        *currentOffset_ = EaseInBounce(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::OutBounce:
        *currentOffset_ = EaseOutBounce(startValue, endValue, currentTime_, maxTime_);
        break;
    case EasingType::InOutBounce:
        *currentOffset_ = EaseInOutBounce(startValue, endValue, currentTime_, maxTime_);
        break;

    //  特殊イージング
    case EasingType::SquishyScaling:
        *currentOffset_ = EaseAmplitudeScale(startValue, currentTime_, maxTime_, amplitude_, period_);
        break;

    }

    // 2フェーズ計算のために一時変更した値を復元
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

    switch (finishValueType_) {
    case EasingFinishValueType::Start:
        if (isStartEndReverse_) {
            *currentOffset_ = endValue_ + baseValue_;
        } else {
            *currentOffset_ = startValue_ + baseValue_;
        }
        break;
    case EasingFinishValueType::End:
        if (isStartEndReverse_) {
            *currentOffset_ = startValue_ + baseValue_;
        } else {
            *currentOffset_ = endValue_ + baseValue_;
        }
        break;
    default:
        break;
    }
}
template <typename T>
void Easing<T>::Easing::SetAdaptValue(T* value) {
    currentOffset_ = value;
}

template <>
template <>
void Easing<float>::SetAdaptValue<float>(Vector2* value) {
    adaptTargetVec2_ = value;
    switch (adaptFloatAxisType_) {
    case AdaptFloatAxisType::X:
        currentOffset_ = &value->x;
        break;
    case AdaptFloatAxisType::Y:
        currentOffset_ = &value->y;
        break;
    default:
        currentOffset_ = &value->x;
        break;
    }
}

template <>
template <>
void Easing<float>::SetAdaptValue<float>(Vector3* value) {
    adaptTargetVec3_ = value;
    switch (adaptFloatAxisType_) {
    case AdaptFloatAxisType::X:
        currentOffset_ = &value->x;
        break;
    case AdaptFloatAxisType::Y:
        currentOffset_ = &value->y;
        break;
    case AdaptFloatAxisType::Z:
        currentOffset_ = &value->z;
        break;
    default:
        currentOffset_ = &value->x;
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
    currentOffset_ = &vector2Proxy_->Get();
}

template <typename T>
void Easing<T>::Easing::SetCurrentOffset(const T& value) {
    *currentOffset_ = value;
}
template <typename T>
bool Easing<T>::IsEasingStarted() const {
    return currentStartTimeOffset_ >= startTimeOffset_;
}

template class Easing<float>;
template class Easing<Vector2>;
template class Easing<Vector3>;
template class Easing<Vector4>;
