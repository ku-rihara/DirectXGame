#include "FormantSpeechRenderer.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>
#include <numbers>

FormantSpeechRenderer::FormantSpeechRenderer() {
    // 声帯音源は振幅1、開大期を長めにして柔らかい声に
    GlottalSource::Param glottal;
    glottal.amplitude  = 1.0;
    glottal.openRatio  = 0.45;
    glottal.closeRatio = 0.15;
    glottal_.SetParam(glottal);
}

std::vector<double> FormantSpeechRenderer::Render(const std::vector<SpeechSegment>& segments, int sampleRate) {
    ResetState();

    size_t total = 0;
    for (const auto& segment : segments) {
        total += static_cast<size_t>(std::lround(segment.duration * sampleRate));
    }
    std::vector<double> output;
    output.reserve(total);

    const int interval = (std::max)(param_.updateInterval, 1);
    SpeechParams params;

    for (const auto& segment : segments) {
        const size_t length = static_cast<size_t>(std::lround(segment.duration * sampleRate));
        for (size_t n = 0; n < length; ++n) {
            const double t = (static_cast<double>(n) + 0.5) / static_cast<double>(length);

            // 係数の再計算は間引く(振幅は毎サンプル補間)
            if (n % static_cast<size_t>(interval) == 0) {
                params = segment.start.Lerp(segment.end, t);
                UpdateFilters(params, sampleRate);
            }
            params.voicing    = segment.start.voicing + (segment.end.voicing - segment.start.voicing) * t;
            params.aspiration = segment.start.aspiration + (segment.end.aspiration - segment.start.aspiration) * t;
            params.frication  = segment.start.frication + (segment.end.frication - segment.start.frication) * t;
            params.burst      = segment.start.burst + (segment.end.burst - segment.start.burst) * t;

            const double f0 = segment.start.f0 + (segment.end.f0 - segment.start.f0) * t;
            output.push_back(ProcessSample(params, f0, sampleRate));
        }
    }
    return output;
}

void FormantSpeechRenderer::ResetState() {
    glottal_.ResetPhase();
    for (auto& f : formants_) {
        f.Reset();
    }
    for (auto& f : noiseFilters_) {
        f.Reset();
    }
    nasalPole_.Reset();
    nasalZero_.Reset();
    radiationPrev_ = 0.0;
    jitterValue_   = 0.0;
    vibratoPhase_  = 0.0;
}

void FormantSpeechRenderer::UpdateFilters(const SpeechParams& params, int sampleRate) {
    const double nyquistLimit = sampleRate * 0.45;

    for (size_t i = 0; i < formants_.size(); ++i) {
        const double frequency = (std::min)(params.formant[i] * param_.formantScale, nyquistLimit);
        formants_[i].SetParam(frequency, params.bandwidth[i], sampleRate, false);
    }

    nasalPole_.SetParam(nasalPoleFrequency_, 100.0, sampleRate, false);
    nasalZero_.SetParam(params.nasalZero, 100.0, sampleRate);

    // 摩擦音は声道の大きさの影響を半分だけ受ける
    const double noiseScale = 1.0 + (param_.formantScale - 1.0) * 0.5;
    for (size_t i = 0; i < noiseFilters_.size(); ++i) {
        const double frequency = (std::min)(params.noiseFrequency[i] * noiseScale, nyquistLimit);
        noiseFilters_[i].SetParam(frequency, params.noiseBandwidth[i], sampleRate, true);
    }
}

double FormantSpeechRenderer::ProcessSample(const SpeechParams& params, double f0, int sampleRate) {
    // 高さのゆらぎ(ゆっくり変化するランダム)とビブラート
    jitterValue_ += (jitterNoise_.NextSample() - jitterValue_) * 0.002;
    vibratoPhase_ += param_.vibratoRate / sampleRate;
    vibratoPhase_ -= std::floor(vibratoPhase_);
    const double vibrato = std::sin(2.0 * std::numbers::pi * vibratoPhase_);
    f0 *= 1.0 + param_.vibratoDepth * vibrato + param_.jitter * jitterValue_ * 4.0;

    // 声道(カスケード): 有声音源 + 気息ノイズ
    const double voiced     = glottal_.NextSample(f0, sampleRate) * params.voicing * param_.voicingGain;
    const double aspiration = noise_.NextSample() * params.aspiration * param_.aspirationGain;

    double x = nasalPole_.ProcessSample(voiced + aspiration);
    x        = nasalZero_.ProcessSample(x);
    for (size_t i = formants_.size(); i-- > 0;) {
        x = formants_[i].ProcessSample(x);
    }

    // 口からの放射(高域を持ち上げる差分)
    const double radiated = x - radiationPrev_;
    radiationPrev_        = x;

    // 摩擦・破裂(パラレル)
    const double noiseIn = noise_.NextSample() * (params.frication * param_.fricationGain + params.burst * param_.burstGain);
    const double fric    = noiseFilters_[0].ProcessSample(noiseIn) + 0.6 * noiseFilters_[1].ProcessSample(noiseIn);

    return radiated + fric;
}
