#include "OscillatorSource.h"

using namespace KetaEngine;
#include <cmath>
#include <numbers>

OscillatorSource::OscillatorSource() : OscillatorSource(Param{}) {}

OscillatorSource::OscillatorSource(const Param& param) : param_(param), random_(std::random_device{}()) {}

std::vector<double> OscillatorSource::Generate(size_t length, int sampleRate) {
    std::vector<double> result(length, 0.0);
    if (sampleRate <= 0) {
        return result;
    }

    const double deltaPhase = param_.frequency / sampleRate;
    double phase            = 0.0;
    for (size_t n = 0; n < length; ++n) {
        result[n] = param_.amplitude * Sample(phase);
        phase += deltaPhase;
        phase -= std::floor(phase);
    }
    return result;
}

double OscillatorSource::Sample(double phase) {
    const double twoPi = 2.0 * std::numbers::pi;
    const double saw   = 2.0 * (phase - std::floor(phase + 0.5));

    switch (param_.waveform) {
    case Waveform::Sine:
        return std::sin(twoPi * phase);
    case Waveform::Sawtooth:
        return saw;
    case Waveform::Square:
        return (phase < 0.5) ? 1.0 : -1.0;
    case Waveform::Triangle:
        return 1.0 - 4.0 * std::abs(phase - 0.5);
    case Waveform::SineSaw:
        return 0.5 * std::sin(twoPi * phase) + 0.5 * saw;
    case Waveform::Noise:
        return noiseDist_(random_);
    default:
        return 0.0;
    }
}

const char* OscillatorSource::GetWaveformName(Waveform waveform) const {
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
