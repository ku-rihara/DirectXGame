#include "IIRFilter.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>
#include <numbers>

void IIRFilter::Setup(const Param& param, int sampleRate) {
    param_ = param;

    // ナイキスト周波数ちょうどはtanが発散するので少し手前でクランプ
    const double fs  = static_cast<double>((std::max)(sampleRate, 1));
    const double fc1 = std::clamp(param_.frequency / fs, 1.0e-5, 0.499);
    const double fc2 = std::clamp(param_.frequency2 / fs, 1.0e-5, 0.499);
    const double q   = (std::max)(param_.q, 1.0e-3);

    Coefficients a, b;
    Design((std::min)(fc1, fc2), (std::max)(fc1, fc2), q, a, b);
    SetCoefficients(a, b);
}

void IIRFilter::SetCoefficients(const Coefficients& a, const Coefficients& b) {
    a_ = a;
    b_ = b;
    Reset();
}

void IIRFilter::Reset() {
    x1_ = x2_ = 0.0;
    y1_ = y2_ = 0.0;
}

double IIRFilter::ProcessSample(double x) {
    // y(n) = Σb(m)x(n-m) - Σa(m)y(n-m)
    const double y = b_[0] * x + b_[1] * x1_ + b_[2] * x2_ - a_[1] * y1_ - a_[2] * y2_;
    x2_            = x1_;
    x1_            = x;
    y2_            = y1_;
    y1_            = y;
    return y;
}

void IIRFilter::Design(double fc1, double fc2, double q, Coefficients& a, Coefficients& b) const {
    const double pi  = std::numbers::pi;
    const double pi2 = pi * pi;

    // デジタル周波数からアナログ周波数へ(プリワーピング)
    const double f1 = std::tan(pi * fc1) / (2.0 * pi);
    const double f2 = std::tan(pi * fc2) / (2.0 * pi);

    double a0 = 1.0;
    switch (param_.type) {
    case Type::LowPass:
        a0   = 1.0 + 2.0 * pi * f1 / q + 4.0 * pi2 * f1 * f1;
        a[1] = (8.0 * pi2 * f1 * f1 - 2.0) / a0;
        a[2] = (1.0 - 2.0 * pi * f1 / q + 4.0 * pi2 * f1 * f1) / a0;
        b[0] = 4.0 * pi2 * f1 * f1 / a0;
        b[1] = 8.0 * pi2 * f1 * f1 / a0;
        b[2] = 4.0 * pi2 * f1 * f1 / a0;
        break;

    case Type::HighPass:
        a0   = 1.0 + 2.0 * pi * f1 / q + 4.0 * pi2 * f1 * f1;
        a[1] = (8.0 * pi2 * f1 * f1 - 2.0) / a0;
        a[2] = (1.0 - 2.0 * pi * f1 / q + 4.0 * pi2 * f1 * f1) / a0;
        b[0] = 1.0 / a0;
        b[1] = -2.0 / a0;
        b[2] = 1.0 / a0;
        break;

    case Type::BandPass:
        a0   = 1.0 + 2.0 * pi * (f2 - f1) + 4.0 * pi2 * f1 * f2;
        a[1] = (8.0 * pi2 * f1 * f2 - 2.0) / a0;
        a[2] = (1.0 - 2.0 * pi * (f2 - f1) + 4.0 * pi2 * f1 * f2) / a0;
        b[0] = 2.0 * pi * (f2 - f1) / a0;
        b[1] = 0.0;
        b[2] = -2.0 * pi * (f2 - f1) / a0;
        break;

    case Type::BandEliminate:
        a0   = 1.0 + 2.0 * pi * (f2 - f1) + 4.0 * pi2 * f1 * f2;
        a[1] = (8.0 * pi2 * f1 * f2 - 2.0) / a0;
        a[2] = (1.0 - 2.0 * pi * (f2 - f1) + 4.0 * pi2 * f1 * f2) / a0;
        b[0] = (4.0 * pi2 * f1 * f2 + 1.0) / a0;
        b[1] = (8.0 * pi2 * f1 * f2 - 2.0) / a0;
        b[2] = (4.0 * pi2 * f1 * f2 + 1.0) / a0;
        break;

    case Type::Resonator:
        a0   = 1.0 + 2.0 * pi * f1 / q + 4.0 * pi2 * f1 * f1;
        a[1] = (8.0 * pi2 * f1 * f1 - 2.0) / a0;
        a[2] = (1.0 - 2.0 * pi * f1 / q + 4.0 * pi2 * f1 * f1) / a0;
        b[0] = 2.0 * pi * f1 / q / a0;
        b[1] = 0.0;
        b[2] = -2.0 * pi * f1 / q / a0;
        break;

    default:
        a = {1.0, 0.0, 0.0};
        b = {1.0, 0.0, 0.0};
        break;
    }
    a[0] = 1.0;
}

const char* IIRFilter::GetTypeName(Type type) const {
    switch (type) {
    case Type::LowPass:
        return "LowPass";
    case Type::HighPass:
        return "HighPass";
    case Type::BandPass:
        return "BandPass";
    case Type::BandEliminate:
        return "BandEliminate";
    case Type::Resonator:
        return "Resonator";
    default:
        return "Unknown";
    }
}
