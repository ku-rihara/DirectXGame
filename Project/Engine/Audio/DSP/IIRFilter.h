#pragma once

#include "AudioFilter.h"
#include <array>
#include <cmath>

namespace KetaEngine {

/// <summary>
/// IIRフィルター(双2次フィルター、双一次変換で設計)
/// </summary>
class IIRFilter : public AudioFilter {
public:
    enum class Type {
        LowPass,       //< 低域通過(frequency, q)
        HighPass,      //< 高域通過(frequency, q)
        BandPass,      //< 帯域通過(frequency～frequency2)
        BandEliminate, //< 帯域阻止(frequency～frequency2)
        Resonator,     //< 共振器(frequency, q)
        Count,
    };

    struct Param {
        Type type         = Type::LowPass;
        double frequency  = 1000.0;              //< 遮断/中心周波数[Hz](帯域系は下側)
        double frequency2 = 2000.0;              //< 帯域系の上側の遮断周波数[Hz]
        double q          = 1.0 / std::sqrt(2.0); //< クオリティファクタ
    };

    using Coefficients = std::array<double, 3>;

public:
    IIRFilter() = default;
    IIRFilter(const Param& param, int sampleRate) { Setup(param, sampleRate); }

    // パラメータから係数を設計
    void Setup(const Param& param, int sampleRate);

    // 係数を直接指定(a[0]は1として扱う)
    void SetCoefficients(const Coefficients& a, const Coefficients& b);

    void Reset() override;
    double ProcessSample(double x) override;

    /// <summary>
    /// 係数設計
    /// </summary>
    /// <param name="type">フィルターの種類</param>
    /// <param name="fc1">正規化周波数(f/fs)</param>
    /// <param name="fc2">正規化周波数2(帯域系の上側)</param>
    /// <param name="q">クオリティファクタ</param>
    /// <param name="a">出力側係数</param>
    /// <param name="b">入力側係数</param>
    static void Design(Type type, double fc1, double fc2, double q, Coefficients& a, Coefficients& b);

    static const char* GetTypeName(Type type);

private:
    Param param_;
    Coefficients a_ = {1.0, 0.0, 0.0};
    Coefficients b_ = {1.0, 0.0, 0.0};

    // 遅延器
    double x1_ = 0.0, x2_ = 0.0;
    double y1_ = 0.0, y2_ = 0.0;

public:
    const Param& GetParam() const { return param_; }
    const Coefficients& GetA() const { return a_; }
    const Coefficients& GetB() const { return b_; }
};

}; // KetaEngine
