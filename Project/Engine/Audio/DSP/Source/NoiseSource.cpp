#include "NoiseSource.h"

using namespace KetaEngine;

NoiseSource::NoiseSource(double amplitude) : amplitude_(amplitude), random_(std::random_device{}()) {}

std::vector<double> NoiseSource::Generate(size_t length, int /*sampleRate*/) {
    std::vector<double> result(length);
    for (double& s : result) {
        s = NextSample();
    }
    return result;
}

double NoiseSource::NextSample() {
    return amplitude_ * dist_(random_);
}
