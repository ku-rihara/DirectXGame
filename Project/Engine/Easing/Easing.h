#pragma once
#include "utility/EasingCreator/EasingParameterData.h"
#include "Vector2Proxy.h"
// std
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
class Easing {
public:
    Easing()  = default;
    ~Easing() = default;

    /// =========================================================================
    /// Public Methods
    /// =========================================================================

    void Init(const std::string& name, const std::string& adaptFile = "");
    void Reset();
    void ResetStartValue();

    //  Easing setting
    void SettingValue(const EasingParameter<T>& easingParam);

    // 適応
    void ApplyFromJson(const std::string& fileName);
    void ApplyForImGui();

    void SaveAppliedJsonFileName();
    void LoadAndApplyFromSavedJson();

    // イージング更新
    void Update(const float& deltaTime);

    // 変数に適応
    void SetAdaptValue(T* value);

    // 軸の適応
    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, float>, void>
    SetAdaptValue(Vector2* value);

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, float>, void>
    SetAdaptValue(Vector3* value);

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, Vector2>, void>
    SetAdaptValue(Vector3* value);

    // callbacks
    void SetFinishValueType(const EasingFinishValueType& type) { finishValueType_ = type; }
    void SetOnFinishCallback(const std::function<void()>& callback) { onFinishCallback_ = callback; }
    void SetOnWaitEndCallback(const std::function<void()>& callback) { onWaitEndCallback_ = callback; }

private:
    /// =========================================================================
    /// Private Methods
    /// =========================================================================

    void CalculateValue();
    void FinishBehavior();
    void ChangeAdaptAxis();
    void FilePathChangeForType();
    bool IsEasingStarted() const;

private:
    // イージング初期化パラメータ
    EasingType type_                       = EasingType::InSine;
    EasingFinishValueType finishValueType_ = EasingFinishValueType::End;

    AdaptFloatAxisType adaptFloatAxisType_     = AdaptFloatAxisType::X;
    AdaptVector2AxisType adaptVector2AxisType_ = AdaptVector2AxisType::XY;

private:
    // イージングの値
    T startValue_;
    T endValue_;
    T baseValue_;
    T* currentValue_;

    // タイム
    float maxTime_     = 0.0f;
    float currentTime_ = 0.0f;
    float waitTimeMax_ = 0.0f;
    float waitTime_    = 0.0f;

    // オフセット
    float startTimeOffset_        = 0.0f;
    float finishTimeOffset_       = 0.0f;
    float currentStartTimeOffset_ = 0.0f;

    // amplitude用Parameter
    float amplitude_ = 0.0f;
    float period_    = 0.0f;
    float backRatio_ = 0.0f;

    bool isFinished_ = false;
    bool isPlaying_  = false;

private:
    // ファイル
    const std::string FilePath_      = "Resources/EasingParameter/";
    const std::string adaptDataPath_ = "AdaptData/";

    int32_t selectedFileIndex_;
    std::vector<std::string> easingFiles_;
    std::string currentAppliedFileName_;
    std::string filePathForType_;
    std::string currentSelectedFileName_;
    std::string easingName_;

    // 軸
    AdaptVector2AxisType oldTypeVector2_;
    AdaptFloatAxisType oldTypeFloat_;
    Vector2* adaptTargetVec2_ = nullptr;
    Vector3* adaptTargetVec3_ = nullptr;
    std::unique_ptr<IVector2Proxy> vector2Proxy_;

    // コールバック関数
    std::function<void()> onFinishCallback_;
    std::function<void()> onWaitEndCallback_;

public:
    /// -------------------------------------------------------------------------
    /// Getter methods
    /// -------------------------------------------------------------------------
    const T& GetValue() const { return *currentValue_; }
    const T& GetEndValue() const { return endValue_; }
    const bool& IsFinished() const { return isFinished_; }
    const bool& IsPlaying() const { return isPlaying_; }
    const std::string& GetCurrentAppliedFileName() const { return currentAppliedFileName_; }
    const float& GetCurrentEaseTime() const { return currentTime_; }
    /// -------------------------------------------------------------------------
    /// Setter methods
    /// -------------------------------------------------------------------------
    void SetStartValue(const T& value) { startValue_ = value; }
    void SetEndValue(const T& value) { endValue_ = value; }
    void SetCurrentValue(const T& value);
    void SetBaseValue(const T& value) { baseValue_ = value; }
    void SetType(const EasingType& type) { type_ = type; }
    void SetMaxTime(const float& time) { maxTime_ = time; }
};