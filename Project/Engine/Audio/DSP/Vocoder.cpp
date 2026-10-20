#include "Vocoder.h"

using namespace KetaEngine;
#include "FFT.h"
#include <algorithm>

AudioPCM Vocoder::Process(const AudioPCM& carrier, const AudioPCM& modulator) const {
    const size_t length = (std::min)(carrier.GetLength(), modulator.GetLength());
    AudioPCM result(carrier.GetSampleRate(), carrier.GetChannelCount(), length);
    if (length == 0) {
        return result;
    }

    const std::vector<double> modulatorMono = modulator.GetMonoMix();
    for (int c = 0; c < carrier.GetChannelCount(); ++c) {
        std::vector<double> processed = ProcessChannel(carrier.GetChannel(c), modulatorMono);
        processed.resize(length, 0.0);
        result.GetChannel(c) = std::move(processed);
    }

    if (param_.normalize) {
        result.Normalize(param_.normalizePeak);
    }
    return result;
}

std::vector<double> Vocoder::ProcessChannel(const std::vector<double>& carrier, const std::vector<double>& modulator) const {
    const size_t n         = FFT::NextPowerOfTwo(static_cast<size_t>((std::max)(param_.fftSize, 4)));
    const size_t hop       = n / 2;
    const size_t length    = (std::min)(carrier.size(), modulator.size());
    const size_t bandWidth = std::clamp(static_cast<size_t>((std::max)(param_.bandWidth, 1)), size_t(1), hop);

    std::vector<double> output(length, 0.0);
    if (length < n) {
        return output;
    }

    const std::vector<double> w = WindowFunction::Generate(param_.window, static_cast<int>(n));
    std::vector<FFT::Complex> x(n), b(n), y(n);
    std::vector<double> envelope(n);

    const size_t frameCount = (length - hop) / hop;
    for (size_t frame = 0; frame < frameCount; ++frame) {
        const size_t offset = hop * frame;

        // X(k):キャリアのスペクトル / B(k):モジュレータのスペクトル
        for (size_t i = 0; i < n; ++i) {
            x[i] = FFT::Complex(carrier[offset + i] * w[i], 0.0);
            b[i] = FFT::Complex(modulator[offset + i] * w[i], 0.0);
        }
        FFT::Transform(x, false);
        FFT::Transform(b, false);

        // モジュレータの振幅スペクトルを帯域ごとに平均化して包絡を求める
        std::fill(envelope.begin(), envelope.end(), 0.0);
        for (size_t bandStart = 0; bandStart < hop; bandStart += bandWidth) {
            const size_t bandEnd = (std::min)(bandStart + bandWidth, hop);
            double average       = 0.0;
            for (size_t k = bandStart; k < bandEnd; ++k) {
                average += std::abs(b[k]);
            }
            average /= static_cast<double>(bandEnd - bandStart);
            for (size_t k = bandStart; k < bandEnd; ++k) {
                envelope[k] = average;
            }
        }
        envelope[0]   = 0.0; // 直流成分
        envelope[hop] = 0.0; // ナイキスト
        for (size_t k = 1; k < hop; ++k) {
            envelope[n - k] = envelope[k]; // 負の周波数側へ折り返し
        }

        // フィルタリング → 逆FFT
        for (size_t k = 0; k < n; ++k) {
            y[k] = x[k] * envelope[k];
        }
        FFT::Transform(y, true);

        // オーバーラップアド
        for (size_t i = 0; i < n; ++i) {
            output[offset + i] += y[i].real();
        }
    }
    return output;
}
