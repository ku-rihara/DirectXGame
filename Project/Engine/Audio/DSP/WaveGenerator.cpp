#include "WaveGenerator.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>

AudioPCM WaveGenerator::Generate(const Param& param, int sampleRate, int channelCount) {
    const size_t length = static_cast<size_t>((std::max)(param.duration, 0.0) * sampleRate);
    AudioPCM pcm(sampleRate, channelCount, length);

    const double twoPi      = 2.0 * std::numbers::pi;
    const double deltaPhase = param.frequency / sampleRate;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> noiseDist(-1.0, 1.0);

    std::vector<double> wave(length);
    double phase = 0.0;
    for (size_t n = 0; n < length; ++n) {
        double value = 0.0;
        switch (param.waveform) {
        case Waveform::Sine:
            value = std::sin(twoPi * phase);
            break;
        case Waveform::Sawtooth:
            value = 2.0 * (phase - std::floor(phase + 0.5));
            break;
        case Waveform::Square:
            value = (phase < 0.5) ? 1.0 : -1.0;
            break;
        case Waveform::Triangle:
            value = 1.0 - 4.0 * std::abs(phase - 0.5);
            break;
        case Waveform::SineSaw:
            value = 0.5 * std::sin(twoPi * phase) + 0.5 * (2.0 * (phase - std::floor(phase + 0.5)));
            break;
        case Waveform::Noise:
            value = noiseDist(rng);
            break;
        default:
            break;
        }
        wave[n] = param.amplitude * value;

        phase += deltaPhase;
        phase -= std::floor(phase);
    }

    for (int c = 0; c < pcm.GetChannelCount(); ++c) {
        pcm.GetChannel(c) = wave;
    }
    pcm.ApplyFade(param.fadeTime, param.fadeTime);
    return pcm;
}

std::vector<double> WaveGenerator::GlottalPulse(int sampleRate, double f0, double duration, double amplitude,
    double openRatio, double closeRatio) {

    const size_t length = static_cast<size_t>((std::max)(duration, 0.0) * sampleRate);
    std::vector<double> result(length, 0.0);
    if (f0 <= 0.0) {
        return result;
    }

    const double pi = std::numbers::pi;
    openRatio       = std::clamp(openRatio, 0.01, 0.98);
    closeRatio      = std::clamp(closeRatio, 0.01, 1.0 - openRatio);

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
        result[n] = amplitude * value;

        phase += f0 / sampleRate;
        phase -= std::floor(phase);
    }
    return result;
}

const char* WaveGenerator::GetWaveformName(Waveform waveform) {
    switch (waveform) {
    case Waveform::Sine:
        return "Sine";
    case Waveform::Sawtooth:
        return "Sawtooth";
    case Waveform::Square:
        return "Square";
    case Waveform::Triangle:
        return "Triangle";
    case Waveform::SineSaw:
        return "SineSaw";
    case Waveform::Noise:
        return "Noise";
    default:
        return "Unknown";
    }
}
