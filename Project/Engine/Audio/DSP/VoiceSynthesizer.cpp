#include "VoiceSynthesizer.h"

using namespace KetaEngine;
#include "IIRFilter.h"
#include "WaveGenerator.h"
#include <algorithm>
#include <cctype>
#include <string_view>

VoiceSynthesizer::VoiceSynthesizer() {
    // 母音のフォルマント
    vowelMap_["a"] = {800, 1200, 2500, 3500};
    vowelMap_["i"] = {300, 2300, 2900, 3500};
    vowelMap_["u"] = {300, 1200, 2500, 3500};
    vowelMap_["e"] = {500, 1900, 2500, 3500};
    vowelMap_["o"] = {500, 800, 2500, 3500};

    // 子音のフォルマント(仮の値)
    consonantMap_["k"] = {300, 1500, 2500, 3500};
    consonantMap_["s"] = {500, 2000, 3000, 4000};
    consonantMap_["t"] = {600, 2200, 3200, 4200};
    consonantMap_["n"] = {700, 2400, 3400, 4400};
}

AudioPCM VoiceSynthesizer::Synthesize(const std::string& text) const {
    struct Mora {
        std::string vowel;
        std::string consonant;
    };

    // テキストを1音ずつに分解
    std::vector<Mora> moras;
    std::string consonant;
    for (char raw : text) {
        const char c = static_cast<char>(std::tolower(static_cast<unsigned char>(raw)));

        if (c == '-' || c == ' ') {
            // 無音1拍
            moras.push_back({});
            consonant.clear();
        } else if (IsVowel(c)) {
            moras.push_back({std::string(1, c), consonant});
            consonant.clear();
        } else if (IsConsonant(c)) {
            if (consonant == "n" && c != 'y') {
                // 「ん」
                moras.push_back({"", "n"});
                consonant = std::string(1, c);
            } else if (!consonant.empty() && consonant.back() == c) {
                // 「っ」(促音)は無音1拍
                moras.push_back({});
                consonant = std::string(1, c);
            } else {
                consonant += c;
            }
        }
    }
    if (consonant == "n") {
        moras.push_back({"", "n"});
    }

    const size_t moraLength = static_cast<size_t>(param_.sampleRate * param_.moraDuration);
    AudioPCM mono(param_.sampleRate, 1, moraLength * moras.size());

    // 1音ずつ合成して並べる
    size_t position = 0;
    for (const Mora& mora : moras) {
        const std::vector<double> wave = SynthesizeMora(mora.vowel, mora.consonant);
        auto& dst                      = mono.GetChannel(0);
        for (size_t n = 0; n < wave.size() && position + n < dst.size(); ++n) {
            dst[position + n] += wave[n];
        }
        position += moraLength;
    }

    if (param_.normalize) {
        mono.Normalize(param_.normalizePeak);
    }

    // 指定チャンネル数に複製
    AudioPCM result(param_.sampleRate, param_.channelCount, mono.GetLength());
    result.MixFrom(mono, 0);
    return result;
}

std::vector<double> VoiceSynthesizer::SynthesizeMora(const std::string& vowel, const std::string& consonant) const {
    const int fs          = param_.sampleRate;
    const size_t length   = static_cast<size_t>(fs * param_.moraDuration);
    std::vector<double> output(length, 0.0);
    if (vowel.empty() && consonant.empty()) {
        return output;
    }

    // 声門音源
    const std::vector<double> source = WaveGenerator::GlottalPulse(fs, param_.pitch, param_.moraDuration, 0.2,
        param_.openRatio, param_.closeRatio);

    // 子音(見つからなければ先頭1文字で再検索、それも無ければ母音のみ)
    if (!consonant.empty()) {
        auto it = consonantMap_.find(consonant);
        if (it == consonantMap_.end()) {
            it = consonantMap_.find(consonant.substr(0, 1));
        }
        if (it != consonantMap_.end()) {
            ApplyFormant(source, it->second, param_.consonantFormantScale, param_.consonantGain, output);
        }
    }

    // 母音
    if (auto it = vowelMap_.find(vowel); it != vowelMap_.end()) {
        ApplyFormant(source, it->second, param_.vowelFormantScale, 1.0, output);
    }

    // ディエンファシス(口からの放射特性を戻す)
    for (size_t n = 1; n < length; ++n) {
        output[n] += param_.deEmphasis * output[n - 1];
    }

    // フェードイン/アウト
    const size_t fade = (std::min)(length / 2, static_cast<size_t>(fs * param_.fadeTime));
    for (size_t n = 0; n < fade; ++n) {
        const double t          = static_cast<double>(n) / fade;
        output[n]              *= t;
        output[length - n - 1] *= t;
    }
    return output;
}

void VoiceSynthesizer::ApplyFormant(const std::vector<double>& source, const Formant& formant, double scale, double gain,
    std::vector<double>& output) const {

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

bool VoiceSynthesizer::IsVowel(char c) {
    return std::string_view("aiueo").find(c) != std::string_view::npos;
}

bool VoiceSynthesizer::IsConsonant(char c) {
    return std::string_view("kstnhmyrwgzdbpcfjv").find(c) != std::string_view::npos;
}
