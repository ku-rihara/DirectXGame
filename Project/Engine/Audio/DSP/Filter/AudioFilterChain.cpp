#include "AudioFilterChain.h"

using namespace KetaEngine;

void AudioFilterChain::Reset() {
    for (auto& filter : filters_) {
        filter->Reset();
    }
}

double AudioFilterChain::ProcessSample(double x) {
    for (auto& filter : filters_) {
        x = filter->ProcessSample(x);
    }
    return x;
}

IAudioFilter* AudioFilterChain::Add(std::unique_ptr<IAudioFilter> filter) {
    filters_.push_back(std::move(filter));
    return filters_.back().get();
}
