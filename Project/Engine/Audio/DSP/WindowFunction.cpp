#include "WindowFunction.h"

using namespace KetaEngine;
#include <cmath>
#include <numbers>

std::vector<double> WindowFunction::Generate(Type type, int size) {
    if (size <= 0) {
        return {};
    }
    std::vector<double> result(static_cast<size_t>(size), 1.0);

    // 偶数は周期的な窓、奇数は中心が1になるよう半サンプルずらす
    const double offset = (size % 2 == 0) ? 0.0 : 0.5;
    const double twoPi  = 2.0 * std::numbers::pi;

    for (int n = 0; n < size; ++n) {
        const double t = twoPi * (n + offset) / size;
        switch (type) {
        case Type::Hanning:
            result[n] = 0.5 - 0.5 * std::cos(t);
            break;
        case Type::Hamming:
            result[n] = 0.54 - 0.46 * std::cos(t);
            break;
        case Type::Blackman:
            result[n] = 0.42 - 0.5 * std::cos(t) + 0.08 * std::cos(2.0 * t);
            break;
        default:
            break;
        }
    }
    return result;
}

const char* WindowFunction::GetTypeName(Type type) {
    switch (type) {
    case Type::Rectangular:
        return "Rectangular";
    case Type::Hanning:
        return "Hanning";
    case Type::Hamming:
        return "Hamming";
    case Type::Blackman:
        return "Blackman";
    default:
        return "Unknown";
    }
}
