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

enum class AdaptFloatAxisType {
    X, // X軸
    Y, // Y軸
    Z, // Z軸
    COUNT,
};

enum class AdaptVector2AxisType {
    XY,
    XZ,
    YZ,
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

constexpr std::array<const char*, static_cast<int>(AdaptFloatAxisType::COUNT)> AdaptFloatAxisTypeLabels = {
    "X",
    "Y",
    "Z",
};

constexpr std::array<const char*, static_cast<int>(AdaptVector2AxisType::COUNT)> AdaptVector2AxisTypeLabels = {
    "XY",
    "XZ",
    "YZ",
};

constexpr std::array<const char*, static_cast<int>(AdaptVector2AxisType::COUNT)> FinishTypeLabels = {
    "Start", "End"};

/// <summary>
/// イージングタイプをImGuiでセレクトする
/// </summary>
/// <param name="label"></param>
/// <param name="target"></param>
void ImGuiEasingTypeSelector(const char* label, int32_t& target);

namespace KetaEngine {

template <typename T>
struct EasingParameter {

    EasingType type                  = EasingType::InSine;
    EasingFinishValueType finishType = EasingFinishValueType::End;

    AdaptFloatAxisType adaptFloatAxisType  = AdaptFloatAxisType::X;
    AdaptVector2AxisType adaptVec2AxisType = AdaptVector2AxisType::XY;

    T startValue = {};
    T endValue   = {};

    float maxTime        = 0.0f;
    float returnMaxTime  = 0.0f;                // 戻りフェーズの時間（0=戻りなし）
    EasingType returnType = EasingType::InSine; // 戻りフェーズのイージング種類
    float amplitude = 0.0f;
    float period    = 0.0f;

    float waitTimeMax      = 0.0f;
    float finishOffsetTime = 0.0f;

    float startTimeOffset = 0.0f;
};

}; // KetaEngine
