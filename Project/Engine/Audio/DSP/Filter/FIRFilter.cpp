#include "FIRFilter.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>
#include <numbers>

void FIRFilter::Setup(const Param& param, int sampleRate) {
    param_ = param;

    // 遅延器の数は偶数に揃える
    param_.delayCount = (std::max)(param_.delayCount, 2);
    if (param_.delayCount % 2 != 0) {
        ++param_.delayCount;
    }

    const double fs  = static_cast<double>((std::max)(sampleRate, 1));
    const double fe1 = std::clamp(param_.frequency / fs, 0.0, 0.5);
    const double fe2 = std::clamp(param_.frequency2 / fs, 0.0, 0.5);

    SetCoefficients(Design((std::min)(fe1, fe2), (std::max)(fe1, fe2)));
}

void FIRFilter::SetCoefficients(const std::vector<double>& coefficients) {
    coefficients_ = coefficients;
    history_.assign(coefficients_.size(), 0.0);
    writePos_ = 0;
}

void FIRFilter::Reset() {
    std::fill(history_.begin(), history_.end(), 0.0);
    writePos_ = 0;
}

double FIRFilter::ProcessSample(double x) {
    const size_t size = coefficients_.size();
    if (size == 0) {
        return x;
    }

    // y(n) = Σ b(m) x(n-m)
    history_[writePos_] = x;
    double y            = 0.0;
    size_t index        = writePos_;
    for (size_t m = 0; m < size; ++m) {
        y += coefficients_[m] * history_[index];
        index = (index == 0) ? size - 1 : index - 1;
    }
    writePos_ = (writePos_ + 1) % size;
    return y;
}

std::vector<double> FIRFilter::Design(double fe1, double fe2) const {
    const int tapCount = param_.delayCount + 1;
    const int half     = param_.delayCount / 2;
    const double pi    = std::numbers::pi;

    std::vector<double> b(static_cast<size_t>(tapCount), 0.0);
    const std::vector<double> w = WindowFunction(param_.window).Generate(tapCount);

    // -J/2～J/2まで理想特性のインパルス応答を求めて窓を掛ける
    for (int m = -half; m <= half; ++m) {
        double value = 0.0;
        switch (param_.type) {
        case Type::LowPass:
            value = 2.0 * fe1 * Sinc(2.0 * pi * fe1 * m);
            break;
        case Type::HighPass:
            value = Sinc(pi * m) - 2.0 * fe1 * Sinc(2.0 * pi * fe1 * m);
            break;
        case Type::BandPass:
            value = 2.0 * fe2 * Sinc(2.0 * pi * fe2 * m) - 2.0 * fe1 * Sinc(2.0 * pi * fe1 * m);
            break;
        case Type::BandEliminate:
            value = Sinc(pi * m) - 2.0 * fe2 * Sinc(2.0 * pi * fe2 * m) + 2.0 * fe1 * Sinc(2.0 * pi * fe1 * m);
            break;
        default:
            break;
        }
        b[half + m] = value * w[half + m];
    }
    return b;
}

const char* FIRFilter::GetTypeName(Type type) const {
    switch (type) {
    case Type::LowPass:
        return "LowPass";
    case Type::HighPass:
        return "HighPass";
    case Type::BandPass:
        return "BandPass";
    case Type::BandEliminate:
        return "BandEliminate";
    default:
        return "Unknown";
    }
}

double FIRFilter::Sinc(double x) const {
    return (x == 0.0) ? 1.0 : std::sin(x) / x;
}
