#pragma once

#include "AudioFilter.h"
#include "WindowFunction.h"
#include <vector>

namespace KetaEngine {

/// <summary>
/// FIRフィルター(窓関数法)
/// </summary>
class FIRFilter : public AudioFilter {
public:
    enum class Type {
        LowPass,       //< 低域通過
        HighPass,      //< 高域通過
        BandPass,      //< 帯域通過
        BandEliminate, //< 帯域阻止
        Count,
    };

    struct Param {
        Type type                  = Type::LowPass;
        double frequency           = 1000.0; //< 遮断周波数[Hz](帯域系は下側)
        double frequency2          = 2000.0; //< 帯域系の上側の遮断周波数[Hz]
        int delayCount             = 64;     //< 遅延器の数J(偶数、タップ数はJ+1)
        WindowFunction::Type window = WindowFunction::Type::Hanning;
    };

public:
    FIRFilter() = default;
    FIRFilter(const Param& param, int sampleRate) { Setup(param, sampleRate); }

    // パラメータから係数を設計
    void Setup(const Param& param, int sampleRate);

    // 係数を直接指定
    void SetCoefficients(const std::vector<double>& coefficients);

    void Reset() override;
    double ProcessSample(double x) override;

    /// <summary>
    /// 係数設計
    /// </summary>
    /// <param name="type">フィルターの種類</param>
    /// <param name="fe1">正規化遮断周波数(f/fs)</param>
    /// <param name="fe2">正規化遮断周波数2(帯域系の上側)</param>
    /// <param name="delayCount">遅延器の数J(偶数)</param>
    /// <param name="window">窓関数</param>
    static std::vector<double> Design(Type type, double fe1, double fe2, int delayCount, WindowFunction::Type window);

    static const char* GetTypeName(Type type);

private:
    static double Sinc(double x);

private:
    Param param_;
    std::vector<double> coefficients_; //< b(m)
    std::vector<double> history_;      //< 入力の遅延器(リングバッファ)
    size_t writePos_ = 0;

public:
    const Param& GetParam() const { return param_; }
    const std::vector<double>& GetCoefficients() const { return coefficients_; }
};

}; // KetaEngine
