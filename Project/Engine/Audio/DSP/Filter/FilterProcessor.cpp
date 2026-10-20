#include "FilterProcessor.h"

using namespace KetaEngine;
#include "Audio/DSP/Core/AudioPCM.h"
#include "IAudioFilter.h"

void FilterProcessor::Apply(IAudioFilter& filter, std::vector<double>& samples) const {
    for (double& s : samples) {
        s = filter.ProcessSample(s);
    }
}

void FilterProcessor::Apply(IAudioFilter& filter, AudioPCM& pcm) const {
    for (int c = 0; c < pcm.GetChannelCount(); ++c) {
        filter.Reset();
        Apply(filter, pcm.GetChannel(c));
    }
    filter.Reset();
}
