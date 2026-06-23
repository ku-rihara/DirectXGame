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
    "SquishyScaling"};

constexpr std::array<const char*, 2> FinishTypeLabels = {
    "Start", "End"};

/// <summary>
/// イージングタイプをImGuiでセレクトする
/// </summary>
void ImGuiEasingTypeSelector(const char* label, int32_t& target);

namespace KetaEngine {

template <typename T>
struct EasingParameter {

    EasingType type                  = EasingType::InSine;
    EasingFinishValueType finishType = EasingFinishValueType::End;

    T startValue = {};
    T endValue   = {};

    float maxTime        = 0.0f;
    float returnMaxTime  = 0.0f;
    EasingType returnType = EasingType::InSine;
    float amplitude = 0.0f;
    float period    = 0.0f;

    float waitTimeMax      = 0.0f;
    float finishOffsetTime = 0.0f;

    float startTimeOffset = 0.0f;
};

}; // KetaEngine
