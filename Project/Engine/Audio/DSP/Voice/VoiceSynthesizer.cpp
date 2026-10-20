#include "VoiceSynthesizer.h"

using namespace KetaEngine;
#include "Audio/DSP/Filter/IIRFilter.h"
#include "Audio/DSP/Source/ISoundSource.h"
#include <algorithm>

AudioPCM VoiceSynthesizer::Synthesize(const std::string& text, ISoundSource& source) const {
    const std::vector<Mora> moras = parser_.Parse(text);
    const size_t moraLength       = static_cast<size_t>(param_.sampleRate * param_.moraDuration);

    // 1音ずつ合成して並べる
    std::vector<double> mono(moraLength * moras.size(), 0.0);
    size_t position = 0;
    for (const Mora& mora : moras) {
        const std::vector<double> wave = SynthesizeMora(mora, source);
        for (size_t n = 0; n < wave.size() && position + n < mono.size(); ++n) {
            mono[position + n] += wave[n];
        }
        position += moraLength;
    }

    // 指定チャンネル数に複製
    AudioPCM result(param_.sampleRate, param_.channelCount, mono.size());
    result.FillAllChannels(mono);
    if (param_.normalize) {
        result.Normalize(param_.normalizePeak);
    }
    return result;
}

std::vector<double> VoiceSynthesizer::SynthesizeMora(const Mora& mora, ISoundSource& source) const {
    const int fs        = param_.sampleRate;
    const size_t length = static_cast<size_t>(fs * param_.moraDuration);
    std::vector<double> output(length, 0.0);
    if (mora.vowel.empty() && mora.consonant.empty()) {
        return output;
    }

    const std::vector<double> excitation = source.Generate(length, fs);

    // 子音(未登録なら母音のみ)
    if (const auto* formant = formantTable_.FindConsonant(mora.consonant)) {
        ApplyFormant(excitation, *formant, param_.consonantFormantScale, param_.consonantGain, output);
    }

    // 母音
    if (const auto* formant = formantTable_.FindVowel(mora.vowel)) {
        ApplyFormant(excitation, *formant, param_.vowelFormantScale, 1.0, output);
    }

    // ディエンファシス(口からの放射特性を戻す)
    for (size_t n = 1; n < length; ++n) {
        output[n] += param_.deEmphasis * output[n - 1];
    }

    // フェードイン/アウト
    const size_t fade = (std::min)(length / 2, static_cast<size_t>(fs * param_.fadeTime));
    for (size_t n = 0; n < fade; ++n) {
        const double t = static_cast<double>(n) / fade;
        output[n] *= t;
        output[length - n - 1] *= t;
    }
    return output;
}

void VoiceSynthesizer::ApplyFormant(const std::vector<double>& source, const FormantTable::Formant& formant, double scale,
    double gain, std::vector<double>& output) const {

    IIRFilter filter;
    for (double frequency : formant) {
        const double center = frequency * scale;

        IIRFilter::Param p;
        p.type       = IIRFilter::Type::BandPass;
        p.frequency  = center - param_.formantBandwidth * 0.5;
        p.frequency2 = center + param_.formantBandwidth * 0.5;
        filter.Setup(p, param_.sampleRate);

        for (size_t n = 0; n < output.size() && n < source.size(); ++n) {
            output[n] += gain * filter.ProcessSample(source[n]);
        }
    }
}
