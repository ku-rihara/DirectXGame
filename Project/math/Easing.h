#pragma once
#include "EasingFunction.h"
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
class Easingw {
public:
    Easingw() = default;

    void Init(const std::string& name);

    void Reset();

    //  Easing setting
    void SettingValue(const EasingParameter<T>& easingParam);

    // 適応
    void ApplyFromJson(const std::string& fileName);
    void ApplyForImGui();

    void SaveAppliedJsonFileName();
    void LoadAndApplyFromSavedJson();

    // イージング更新
    void Update(float deltaTime);
    void UpdateWait(float deltaTime);

    // 変数に適応
    void SetAdaptValue(T* value);

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, float>, void>
    SetAdaptValue(Vector2* value);

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, float>, void>
    SetAdaptValue(Vector3* value);

    template <typename U = T>
    typename std::enable_if_t<std::is_same_v<U, Vector2>, void>
    SetAdaptValue(Vector3* value);

private:
    // イージング計算
    void CalculateValue();
    // 終了処理
    void FinishBehavior();

    void ChangeAdaptAxis();

    void FilePathChangeForType();

private:
    // メンバ変数
    EasingType type_                       = EasingType::InSine;
    EasingFinishValueType finishValueType_ = EasingFinishValueType::End;

    AdaptFloatAxisType adaptFloatAxisType_     = AdaptFloatAxisType::X;
    AdaptVector2AxisType adaptVector2AxisType_ = AdaptVector2AxisType::XY;

    T startValue_;
    T endValue_;
    T* currentValue_;

    float maxTime_     = 0.0f;
    float currentTime_ = 0.0f;

    bool isFinished_ = false;

    // amplitude用Parameter
    float amplitude_ = 0.0f;
    float period_    = 0.0f;
    float backRatio_ = 0.0f;

    //etc
    float waitTimeMax_ = 0.0f;
    float finishTimeOffset_=0.0f;

    float waitTime_ = 0.0f; // 待機時間

private:
    int32_t selectedFileIndex_;
    AdaptVector2AxisType oldTypeVector2_;
    AdaptFloatAxisType oldTypeFloat_;

    std::vector<std::string> easingFiles_;

    const std::string FilePath_ = "Resources/EasingParameter/";
    std::string currentAppliedFileName_;
    std::string filePathForType_;
    std::string currentSelectedFileName_;
    std::string easingName_;

    Vector2* adaptTargetVec2_ = nullptr;
    Vector3* adaptTargetVec3_ = nullptr;
    std::unique_ptr<IVector2Proxy> vector2Proxy_;

    std::function<void()> onFinishCallback_;
    std::function<void()> onWaitEndCallback_;

public:
    /// -------------------------------------------------------------------------
    /// Getter methods
    /// -------------------------------------------------------------------------
    const T& GetValue() const { return *currentValue_; }
    bool IsFinished() const { return isFinished_; }
    std::string GetCurrentAppliedFileName() const { return currentAppliedFileName_; }
    float GetCurrentEaseTime() const { return currentTime_; }
    /// -------------------------------------------------------------------------
    /// Setter methods
    /// -------------------------------------------------------------------------
    /*  void SetEditor(EasingEditor* editor);*/
    void SetFinishValueType(const EasingFinishValueType& type) { finishValueType_ = type; }
    void SetOnFinishCallback(const std::function<void()>& callback) { onFinishCallback_ = callback; }
    void SetOnWaitEndCallback(const std::function<void()>& callback) { onWaitEndCallback_ = callback; }
    void SetStartValue(const T& value) { startValue_ = value; }
    void SetEndValue(const T& value) { endValue_ = value; }
    void SetCurrentValue(const T& value);
};