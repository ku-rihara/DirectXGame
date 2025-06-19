#pragma once
#include "EasingFunction.h"
#include "utility/EasingCreator/EasingCreator.h"
#include "Vector2Proxy.h"
//std
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

template <typename T>
class Easing {
public:
    Easing() = default;

    void Reset();

    //  Easing setting
    void SettingValue(const EasingParameter<T>& easingParam);

    // 適応
    void ApplyFromJson(const std::string& fileName);
    void ApplyForImGui();
    void FilePathChangeForType();

    // イージング更新
    void Update(float deltaTime);

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
   

    void SetValue(const T& value);

private:
    // イージング計算
    void CalculateValue();
    // 終了処理
    void FinishBehavior();

public:
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

    bool isRunning_  = false;
    bool isFinished_ = false;

    // amplitude用Parameter
    float amplitude_ = 0.0f;
    float period_    = 0.0f;
    float backRatio_ = 0.0f;

private:
    int32_t selectedFileIndex_;
    std::vector<std::string> easingFiles_;
    const std::string FilePath_ = "Resources/EasingParameter/";
    std::string filePathForType_;
    std::string currentSelectedFileName_;
    std::unique_ptr<IVector2Proxy> vector2Proxy_;

public:
    /// -------------------------------------------------------------------------
    /// Getter methods
    /// -------------------------------------------------------------------------
    const T& GetValue() const { return *currentValue_; }
    bool IsFinished() const { return isFinished_; }
    bool IsRunning() const { return isRunning_; }

    /// -------------------------------------------------------------------------
    /// Setter methods
    /// -------------------------------------------------------------------------

    void SetFinishValueType(const EasingFinishValueType& type) { finishValueType_ = type; }
};