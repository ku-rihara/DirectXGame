#include "GlottalSource.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>
#include <numbers>

std::vector<double> GlottalSource::Generate(size_t length, int sampleRate) {
    std::vector<double> result(length, 0.0);
    if (param_.pitch <= 0.0 || sampleRate <= 0) {
        return result;
    }

    const double pi         = std::numbers::pi;
    const double openRatio  = std::clamp(param_.openRatio, 0.01, 0.98);
    const double closeRatio = std::clamp(param_.closeRatio, 0.01, 1.0 - openRatio);

    double phase = 0.0; //< 1周期内の位置(0～1)
    for (size_t n = 0; n < length; ++n) {
        double value = 0.0;
        if (phase <= openRatio) {
            // 声門開大期:なめらかに開く
            value = 0.5 * (1.0 - std::cos(pi * phase / openRatio));
        } else if (phase <= openRatio + closeRatio) {
            // 声門閉小期:急速に閉じる
            value = std::cos(0.5 * pi * (phase - openRatio) / closeRatio);
        }
        result[n] = param_.amplitude * value;

        phase += param_.pitch / sampleRate;
        phase -= std::floor(phase);
    }
    return result;
}
