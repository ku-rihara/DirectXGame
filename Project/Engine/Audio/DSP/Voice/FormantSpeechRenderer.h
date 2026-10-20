#pragma once

#include "Audio/DSP/Filter/ResonatorFilter.h"
#include "Audio/DSP/Source/GlottalSource.h"
#include "Audio/DSP/Source/NoiseSource.h"
#include "SpeechParams.h"
#include <array>
#include <vector>

namespace KetaEngine {

/// <summary>
/// Klatt型フォルマント合成器
///  有声音源 + 気息ノイズ → 鼻腔(極/零) → F5～F1の共振器を直列(カスケード)
///  摩擦/破裂ノイズ → 2つの共振器を並列(パラレル)
///  → 口からの放射特性(差分)
/// </summary>
class FormantSpeechRenderer {
public:
    struct Param {
        double formantScale   = 1.0;   //< 声道の短さ(1.15前後で女性、1.3で子供っぽく)
        double vibratoDepth   = 0.006; //< ビブラートの深さ(割合)
        double vibratoRate    = 5.0;   //< ビブラートの速さ[Hz]
        double jitter         = 0.012; //< 高さのゆらぎ(割合)
        double voicingGain    = 1.0;
        double aspirationGain = 0.05;
        double fricationGain  = 0.028;
        double burstGain      = 0.06;
        int updateInterval    = 16; //< 係数を更新するサンプル間隔
    };

public:
    FormantSpeechRenderer();

    // 区間列を波形にする
    std::vector<double> Render(const std::vector<SpeechSegment>& segments, int sampleRate);

private:
    void ResetState();
    void UpdateFilters(const SpeechParams& params, int sampleRate);
    double ProcessSample(const SpeechParams& params, double f0, int sampleRate);

private:
    Param param_;

    // 音源
    GlottalSource glottal_;
    NoiseSource noise_;
    NoiseSource jitterNoise_;

    // 声道(カスケード)
    std::array<ResonatorFilter, 5> formants_;
    ResonatorFilter nasalPole_;
    AntiResonatorFilter nasalZero_;

    // 摩擦/破裂(パラレル)
    std::array<ResonatorFilter, 2> noiseFilters_;

    double radiationPrev_ = 0.0;
    double jitterValue_   = 0.0;
    double vibratoPhase_  = 0.0;

    const double nasalPoleFrequency_ = 270.0;

public:
    Param& GetParam() { return param_; }
    const Param& GetParam() const { return param_; }
    void SetParam(const Param& param) { param_ = param; }
};

}; // KetaEngine
