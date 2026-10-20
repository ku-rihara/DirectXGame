#include "ResonatorFilter.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>
#include <complex>
#include <numbers>

///============================================================
/// ResonatorFilter
///============================================================
void ResonatorFilter::SetParam(double frequency, double bandwidth, int sampleRate, bool normalizePeak) {
    const double fs = static_cast<double>((std::max)(sampleRate, 1));
    const double pi = std::numbers::pi;
    const double t  = 1.0 / fs;
    frequency       = std::clamp(frequency, 1.0, fs * 0.49);
    bandwidth       = (std::max)(bandwidth, 1.0);

    c_ = -std::exp(-2.0 * pi * bandwidth * t);
    b_ = 2.0 * std::exp(-pi * bandwidth * t) * std::cos(2.0 * pi * frequency * t);
    a_ = 1.0 - b_ - c_;

    if (normalizePeak) {
        // 共振周波数での |1 - B e^-jw - C e^-2jw| をAにするとピークゲインが1になる
        const double w                    = 2.0 * pi * frequency * t;
        const std::complex<double> z1     = std::polar(1.0, -w);
        const std::complex<double> denom  = 1.0 - b_ * z1 - c_ * z1 * z1;
        a_                                = std::abs(denom);
    }
}

void ResonatorFilter::Reset() {
    y1_ = y2_ = 0.0;
}

double ResonatorFilter::ProcessSample(double x) {
    const double y = a_ * x + b_ * y1_ + c_ * y2_;
    y2_            = y1_;
    y1_            = y;
    return y;
}

///============================================================
/// AntiResonatorFilter
///============================================================
void AntiResonatorFilter::SetParam(double frequency, double bandwidth, int sampleRate) {
    const double fs = static_cast<double>((std::max)(sampleRate, 1));
    const double pi = std::numbers::pi;
    const double t  = 1.0 / fs;
    frequency       = std::clamp(frequency, 1.0, fs * 0.49);
    bandwidth       = (std::max)(bandwidth, 1.0);

    // 共振器の係数の逆数(直流ゲイン1)
    const double c = -std::exp(-2.0 * pi * bandwidth * t);
    const double b = 2.0 * std::exp(-pi * bandwidth * t) * std::cos(2.0 * pi * frequency * t);
    const double a = 1.0 - b - c;
    a_             = 1.0 / a;
    b_             = -b / a;
    c_             = -c / a;
}

void AntiResonatorFilter::Reset() {
    x1_ = x2_ = 0.0;
}

double AntiResonatorFilter::ProcessSample(double x) {
    const double y = a_ * x + b_ * x1_ + c_ * x2_;
    x2_            = x1_;
    x1_            = x;
    return y;
}
