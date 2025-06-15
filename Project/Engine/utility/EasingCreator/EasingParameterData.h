#pragma once
#include <array>

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
    COUNT,
};

enum class EasingFinishValueType {
    Start, // 開始値
    End, // 終了値
    COUNT,
};

constexpr std::array<const char*, static_cast<int>(EasingType::COUNT)> EasingTypeLabels = {
    "InSine", "OutSine", "InOutSine",
    "InQuint", "OutQuint", "InOutQuint",
    "InCirc", "OutCirc", "InOutCirc",
    "InExpo", "OutExpo", "InOutExpo",
    "InCubic", "OutCubic", "InOutCubic",
    "InQuad", "OutQuad", "InOutQuad",
    "InQuart", "OutQuart", "InOutQuart",
    "InBack", "OutBack", "InOutBack",
    "InBounce", "OutBounce", "InOutBounce",
    "SquishyScaling",
    "BackInSineZero", "BackOutSineZero", "BackInOutSineZero",
    "BackInQuadZero", "BackOutQuadZero", "BackInOutQuadZero",
    "BackInCubicZero", "BackOutCubicZero", "BackInOutCubicZero",
    "BackInQuartZero", "BackOutQuartZero", "BackInOutQuartZero",
    "BackInQuintZero", "BackOutQuintZero", "BackInOutQuintZero",
    "BackInExpoZero", "BackOutExpoZero", "BackInOutExpoZero",
    "BackInCircZero", "BackOutCircZero", "BackInOutCircZero"
};

constexpr const char* FinishTypeLabels[] = {
    "Start", "End"};

template <typename T>
struct EasingParameter {
    EasingType type;
    EasingFinishValueType finishType = EasingFinishValueType::End;
    T startValue;
    T endValue;
    float maxTime   = 0.0f;
    float amplitude = 0.0f;
    float period    = 0.0f;
    float backRatio = 0.0f;
};