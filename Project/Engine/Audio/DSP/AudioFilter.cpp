#include "AudioFilter.h"

using namespace KetaEngine;
#include "AudioPCM.h"

///============================================================
/// AudioFilter
///============================================================
void AudioFilter::Process(std::vector<double>& samples) {
    for (double& s : samples) {
        s = ProcessSample(s);
    }
}

void AudioFilter::Process(AudioPCM& pcm) {
    for (int c = 0; c < pcm.GetChannelCount(); ++c) {
        Reset();
        Process(pcm.GetChannel(c));
    }
    Reset();
}

///============================================================
/// AudioFilterChain
///============================================================
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

AudioFilter* AudioFilterChain::Add(std::unique_ptr<AudioFilter> filter) {
    filters_.push_back(std::move(filter));
    return filters_.back().get();
}

///============================================================
/// AudioFilterBank
///============================================================
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

AudioFilter* AudioFilterBank::Add(std::unique_ptr<AudioFilter> filter) {
    filters_.push_back(std::move(filter));
    return filters_.back().get();
}
