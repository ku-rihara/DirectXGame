#include "AudioPCM.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>

AudioPCM::AudioPCM(int sampleRate, int channelCount, size_t length) {
    Allocate(sampleRate, channelCount, length);
}

void AudioPCM::Allocate(int sampleRate, int channelCount, size_t length) {
    sampleRate_ = sampleRate;
    channels_.assign(static_cast<size_t>((std::max)(channelCount, 1)), std::vector<double>(length, 0.0));
}

void AudioPCM::Resize(size_t length) {
    for (auto& ch : channels_) {
        ch.resize(length, 0.0);
    }
}

void AudioPCM::Clear() {
    for (auto& ch : channels_) {
        std::fill(ch.begin(), ch.end(), 0.0);
    }
}

void AudioPCM::FillAllChannels(const std::vector<double>& wave) {
    for (auto& ch : channels_) {
        ch = wave;
    }
}

void AudioPCM::ApplyGain(double gain) {
    for (auto& ch : channels_) {
        for (double& s : ch) {
            s *= gain;
        }
    }
}

void AudioPCM::Normalize(double peak) {
    double maxAmp = 0.0;
    for (const auto& ch : channels_) {
        for (double s : ch) {
            maxAmp = (std::max)(maxAmp, std::abs(s));
        }
    }
    if (maxAmp > 0.0) {
        ApplyGain(peak / maxAmp);
    }
}

void AudioPCM::ApplyFade(double fadeInSec, double fadeOutSec) {
    const size_t length  = GetLength();
    const size_t fadeIn  = (std::min)(length, static_cast<size_t>((std::max)(fadeInSec, 0.0) * sampleRate_));
    const size_t fadeOut = (std::min)(length, static_cast<size_t>((std::max)(fadeOutSec, 0.0) * sampleRate_));

    for (auto& ch : channels_) {
        for (size_t n = 0; n < fadeIn; ++n) {
            ch[n] *= static_cast<double>(n) / fadeIn;
        }
        for (size_t n = 0; n < fadeOut; ++n) {
            ch[length - n - 1] *= static_cast<double>(n) / fadeOut;
        }
    }
}

void AudioPCM::MixFrom(const AudioPCM& other, size_t destOffset, double gain) {
    if (other.channels_.empty() || destOffset >= GetLength()) {
        return;
    }
    const size_t count = (std::min)(other.GetLength(), GetLength() - destOffset);
    for (size_t c = 0; c < channels_.size(); ++c) {
        const auto& src = other.channels_[c % other.channels_.size()];
        for (size_t n = 0; n < count; ++n) {
            channels_[c][destOffset + n] += src[n] * gain;
        }
    }
}

std::vector<double> AudioPCM::GetMonoMix() const {
    std::vector<double> result(GetLength(), 0.0);
    if (channels_.empty()) {
        return result;
    }
    for (const auto& ch : channels_) {
        for (size_t n = 0; n < result.size(); ++n) {
            result[n] += ch[n];
        }
    }
    for (double& s : result) {
        s /= static_cast<double>(channels_.size());
    }
    return result;
}
