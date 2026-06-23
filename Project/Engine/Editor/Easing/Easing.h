#pragma once
#include "Editor/Easing/EasingParameterData.h"
#include "vector2.h"
#include "vector3.h"
#include <cstdint>
#include <functional>
#include <string>

/// <summary>
/// イージングを扱うテンプレートクラス（純粋計算エンジン）
/// </summary>
namespace KetaEngine {

template <typename T>
class Easing {
public:
    Easing()  = default;
    ~Easing() = default;

    //*----------------------------- public Methods -----------------------------*//

    // 初期化、更新
    void Init(const std::string& adaptFile);
    void Update(float deltaTime);

    // イージングパラメータ、適用先変数を設定
    void SettingValue(const EasingParameter<T>& easingParam);
    void SetAdaptValue(T* value);

    // コールバック関数
    void SetOnFinishCallback(const std::function<void()>& callback) { onFinishCallback_ = callback; }
    void SetOnWaitEndCallback(const std::function<void()>& callback) { onWaitEndCallback_ = callback; }

    // リセット
    void Reset();
    void ResetStartValue();

private:
    //*---------------------------- private Methods ----------------------------*//

    void CalculateValue();
    void FinishBehavior();
    void FilePathChangeForType();
    bool IsEasingStarted() const;

private:
    //*---------------------------- private Variant ----------------------------*//

    // イージング種別
    EasingType type_                       = EasingType::InSine;
    EasingFinishValueType finishValueType_ = EasingFinishValueType::End;

    // 値
    T startValue_   = {};
    T endValue_     = {};
    T baseValue_    = {};
    T* adaptTarget_ = nullptr;

    // 時間
    float maxTime_     = 0.0f;
    float currentTime_ = 0.0f;
    float waitTimeMax_ = 0.0f;
    float waitTime_    = 0.0f;

    // オフセット
    float startTimeOffset_        = 0.0f;
    float finishTimeOffset_       = 0.0f;
    float currentStartTimeOffset_ = 0.0f;

    // 弾性パラメータ
    float amplitude_ = 0.0f;
    float period_    = 0.0f;

    // 戻りフェーズ
    EasingType returnType_ = EasingType::InSine;
    float returnMaxTime_   = 0.0f;
    float forwardMaxTime_  = 0.0f;

    // 状態フラグ
    bool isFinished_ = false;
    bool isPlaying_  = false;

    // コールバック
    std::function<void()> onFinishCallback_;
    std::function<void()> onWaitEndCallback_;

    bool isStartEndReverse_ = false;

    std::string filePathForType_;

public:
    ///========================================================
    /// Getter
    ///========================================================
    const T& GetValue() const { return *adaptTarget_; }
    const T& GetEndValue() const { return endValue_; }
    const T& GetStartValue() const { return startValue_; }
    bool IsFinished() const { return isFinished_; }
    bool IsPlaying() const { return isPlaying_; }
    float GetCurrentEaseTime() const { return currentTime_; }

    ///========================================================
    /// Setter
    ///========================================================
    void SetStartValue(const T& value) { startValue_ = value; }
    void SetStartTimeOffset(float time) { startTimeOffset_ = time; }
    void SetEndValue(const T& value) { endValue_ = value; }
    void SetCurrentOffset(const T& value);
    void SetBaseValue(const T& value) { baseValue_ = value; }
    void SetType(const EasingType& type) { type_ = type; }
    void SetMaxTime(float time) { maxTime_ = time; }
    void SetIsStartEndReverse(bool isReverse) { isStartEndReverse_ = isReverse; }
    void SetFinishTimeOffset(float time) { finishTimeOffset_ = time; }
    void SetAmplitude(float amplitude) { amplitude_ = amplitude; }
    void SetPeriod(float period) { period_ = period; }
};

}; // KetaEngine
