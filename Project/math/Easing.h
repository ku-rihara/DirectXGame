#pragma once
#include "EasingFunction.h"
#include "utility/EasingCreator/EasingCreator.h"

template <typename T>
class Easing {
public:
    Easing() = default;

    void Reset();

    // normal Easing
    void SettingValue(const EasingType& easeType, const T& startValue, const T& endValue, const float& maxTime);
    // back Easing
    void SettingValue(const EasingType& easeType, const T& startValue, const T& endValue, const float& maxTime, const float& backRate);
    // amplitude Easing
    void SettingValue(const EasingType& easeType, const T& startValue, const T& endValue, const float& maxTime, const float& amplitude, const float& period);

    void ApplyFromJson(const nlohmann::json& easingJson);

    // 時間を進めて値を更新
    void Update(float deltaTime);

private:
    // イージング計算
    void CalculateValue();
    // 終了処理
    void FinishBehavior();

public:
    // メンバ変数
    EasingType type_;
    EasingFinishValueType finishValueType_ = EasingFinishValueType::End;
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
    void SetAdaptValue(T* value) { currentValue_ = value; }
    void SetValue(const T& value) { *currentValue_ = value; }
    void SetFinishValueType(const EasingFinishValueType& type) { finishValueType_ = type; }
};