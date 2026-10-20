#include "AudioFilterBank.h"

using namespace KetaEngine;

void AudioFilterBank::Reset() {
    for (auto& filter : filters_) {
        filter->Reset();
    }
}

double AudioFilterBank::ProcessSample(double x) {
    double sum = 0.0;
    for (auto& filter : filters_) {
        sum += filter->ProcessSample(x);
    }
    return sum;
}

IAudioFilter* AudioFilterBank::Add(std::unique_ptr<IAudioFilter> filter) {
    filters_.push_back(std::move(filter));
    return filters_.back().get();
}
