#include "Easing.h"

using namespace KetaEngine;
#include "Editor/Easing/EasingFunction.h"
#include "Function/GetFile.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <imGui.h>
#include <unordered_map>

// ディレクトリキャッシュ
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

    type_            = easingParam.type;
    finishValueType_ = easingParam.finishType;

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

    const std::string stem       = fileName.substr(0, fileName.size() - 5);
    const std::string folderName = "EasingParameter/" + filePathForType_;
    const std::string groupName  = stem;

    auto* gp = GlobalParameter::GetInstance();

    if (!gp->HasGroup(groupName)) {
        gp->CreateGroup(groupName);

        // デフォルト値を登録（ファイルに値があれば LoadFile で上書きされる）
        gp->AddItem(groupName, "type",            static_cast<int32_t>(0));
        gp->AddItem(groupName, "finishType",       static_cast<int32_t>(0));
        gp->AddItem(groupName, "maxTime",          0.0f);
        gp->AddItem(groupName, "returnMaxTime",    0.0f);
        gp->AddItem(groupName, "returnType",       static_cast<int32_t>(0));
        gp->AddItem(groupName, "amplitude",        0.0f);
        gp->AddItem(groupName, "period",           0.0f);
        gp->AddItem(groupName, "finishOffsetTime", 0.0f);
        gp->AddItem(groupName, "waitTime",         0.0f);
        gp->AddItem(groupName, "StartTimeOffset",  0.0f);

        if constexpr (std::is_same_v<T, float>) {
            gp->AddItem(groupName, "startValue", 0.0f);
            gp->AddItem(groupName, "endValue",   1.0f);
        } else if constexpr (std::is_same_v<T, Vector2>) {
            gp->AddItem(groupName, "startValue", Vector2{});
            gp->AddItem(groupName, "endValue",   Vector2{1.0f, 1.0f});
        } else if constexpr (std::is_same_v<T, Vector3>) {
            gp->AddItem(groupName, "startValue", Vector3{});
            gp->AddItem(groupName, "endValue",   Vector3{1.0f, 1.0f, 1.0f});
        } else if constexpr (std::is_same_v<T, Vector4>) {
            gp->AddItem(groupName, "startValue", Vector4{});
            gp->AddItem(groupName, "endValue",   Vector4{1.0f, 1.0f, 1.0f, 1.0f});
        }

        gp->LoadFile(groupName, folderName);
    }

    EasingParameter<T> param;
    param.type            = static_cast<EasingType>(gp->GetValue<int32_t>(groupName, "type"));
    param.finishType      = static_cast<EasingFinishValueType>(gp->GetValue<int32_t>(groupName, "finishType"));
    param.maxTime         = gp->GetValue<float>(groupName, "maxTime");
    param.returnMaxTime   = gp->GetValue<float>(groupName, "returnMaxTime");
    param.returnType      = static_cast<EasingType>(gp->GetValue<int32_t>(groupName, "returnType"));
    param.amplitude       = gp->GetValue<float>(groupName, "amplitude");
    param.period          = gp->GetValue<float>(groupName, "period");
    param.finishOffsetTime = gp->GetValue<float>(groupName, "finishOffsetTime");
    param.waitTimeMax     = gp->GetValue<float>(groupName, "waitTime");
    param.startTimeOffset = gp->GetValue<float>(groupName, "StartTimeOffset");

    if constexpr (std::is_same_v<T, float>) {
        param.startValue = gp->GetValue<float>(groupName, "startValue");
        param.endValue   = gp->GetValue<float>(groupName, "endValue");
    } else if constexpr (std::is_same_v<T, Vector2>) {
        param.startValue = gp->GetValue<Vector2>(groupName, "startValue");
        param.endValue   = gp->GetValue<Vector2>(groupName, "endValue");
    } else if constexpr (std::is_same_v<T, Vector3>) {
        param.startValue = gp->GetValue<Vector3>(groupName, "startValue");
        param.endValue   = gp->GetValue<Vector3>(groupName, "endValue");
    } else if constexpr (std::is_same_v<T, Vector4>) {
        param.startValue = gp->GetValue<Vector4>(groupName, "startValue");
        param.endValue   = gp->GetValue<Vector4>(groupName, "endValue");
    }

    SettingValue(param);

    currentAppliedFileName_ = fileName;
    auto it = std::find(easingFiles_.begin(), easingFiles_.end(), stem);
    if (it != easingFiles_.end()) {
        selectedFileIndex_ = static_cast<int>(std::distance(easingFiles_.begin(), it));
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
    if (!adaptTarget_) {
        return;
    }

    // maxTime_ が 0 の場合は終端値をセットして終了
    if (maxTime_ <= 0.0f) {
        *adaptTarget_ = endValue;
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
        /*    currentOffset_ = EaseInOutQuart(startValue, endValue, currentTime_, maxTime_);*/
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

    //  特殊イージング
    case EasingType::SquishyScaling:
        *adaptTarget_ = EaseAmplitudeScale(startValue, currentTime_, maxTime_, amplitude_, period_);
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

    if (!adaptTarget_) {
        return;
    }

    switch (finishValueType_) {
    case EasingFinishValueType::Start:
        if (isStartEndReverse_) {
            *adaptTarget_ = endValue_ + baseValue_;
        } else {
            *adaptTarget_ = startValue_ + baseValue_;
        }
        break;
    case EasingFinishValueType::End:
        if (isStartEndReverse_) {
            *adaptTarget_ = startValue_ + baseValue_;
        } else {
            *adaptTarget_ = endValue_ + baseValue_;
        }
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
