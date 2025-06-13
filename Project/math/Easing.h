#pragma once
#include "EasingFunction.h"

// EasingTypeの完全版
enum class EasingType {

    // 通常イージング
    InSine,
    OutSine,
    InOutSine,
    InQuint,
    OutQuint,
    InOutQuint,
    InCirc,
    OutCirc,
    InOutCirc,
    InExpo,
    OutExpo,
    InOutExpo,
    InCubic,
    OutCubic,
    InOutCubic,
    InQuad,
    OutQuad,
    InOutQuad,
    InQuart,
    OutQuart,
    InOutQuart,
    InBack,
    OutBack,
    InOutBack,
    InBounce,
    OutBounce,
    InOutBounce,

    // ぷにぷにするイージング
    SquishyScaling,

    // 行って戻ってくるイージング
    BackInSineZero,
    BackOutSineZero,
    BackInOutSineZero,
    BackInQuadZero,
    BackOutQuadZero,
    BackInOutQuadZero,
    BackInCubicZero,
    BackOutCubicZero,
    BackInOutCubicZero,
    BackInQuartZero,
    BackOutQuartZero,
    BackInOutQuartZero,
    BackInQuintZero,
    BackOutQuintZero,
    BackInOutQuintZero,
    BackInExpoZero,
    BackOutExpoZero,
    BackInOutExpoZero,
    BackInCircZero,
    BackOutCircZero,
    BackInOutCircZero,
};

template <typename T>
class Easing {
public:
    Easing() = default;

    // 時間を進めて値を更新
    void Update(float deltaTime) {
        if (!isRunning_ || isFinished_) {
            return;
        }

        currentTime_ += deltaTime;
        if (currentTime_ >= maxTime_) {
            currentTime_ = maxTime_;
            isFinished_  = true;
            isRunning_   = false;
        }

        CalculateValue();
    }

private:
    void CalculateValue() {

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

public:
    // メンバ変数
    EasingType type_;
    T startValue_;
    T endValue_;
    T *currentValue_;

    float maxTime_     = 0.0f;
    float currentTime_ = 0.0f;

    bool isRunning_  = false;
    bool isFinished_ = false;

    // amplitude用Parameter
    float amplitude_ = 0.0f;
    float period_    = 0.0f;
    float backRatio_ = 0.0f;

public:
    const T& GetValue() const { return *currentValue_; }
    bool IsFinished() const { return isFinished_; }
    bool IsRunning() const { return isRunning_; }

    void SetCurrentValue(T* value) { currentValue_ = value; }
};