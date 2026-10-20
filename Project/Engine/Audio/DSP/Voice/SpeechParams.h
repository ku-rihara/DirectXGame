#pragma once

#include <array>

namespace KetaEngine {

/// <summary>
/// ある瞬間の発声パラメータ(Klatt型フォルマント合成器の入力)
/// 振幅はすべて0～1程度の相対値
/// </summary>
struct SpeechParams {
    double f0         = 120.0; //< 基本周波数[Hz]
    double voicing    = 0.0;   //< 有声音源の強さ(AV)
    double aspiration = 0.0;   //< 気息ノイズ(声道を通る、h音など)(AH)
    double frication  = 0.0;   //< 摩擦ノイズ(口先のフィルターを通る、s音など)(AF)
    double burst      = 0.0;   //< 破裂ノイズ(AB)

    std::array<double, 5> formant   = {700.0, 1200.0, 2600.0, 3500.0, 4500.0}; //< フォルマント周波数[Hz]
    std::array<double, 5> bandwidth = {80.0, 100.0, 150.0, 250.0, 300.0};      //< 帯域幅[Hz]

    std::array<double, 2> noiseFrequency = {4000.0, 6000.0}; //< 摩擦/破裂ノイズの共振周波数[Hz]
    std::array<double, 2> noiseBandwidth = {2000.0, 2000.0}; //< 同 帯域幅[Hz]

    double nasalZero = 270.0; //< 鼻腔の反共振周波数[Hz](鼻極と同じ270なら鼻音なし)

    // 線形補間
    SpeechParams Lerp(const SpeechParams& to, double t) const {
        SpeechParams r;
        auto mix   = [t](double a, double b) { return a + (b - a) * t; };
        r.f0         = mix(f0, to.f0);
        r.voicing    = mix(voicing, to.voicing);
        r.aspiration = mix(aspiration, to.aspiration);
        r.frication  = mix(frication, to.frication);
        r.burst      = mix(burst, to.burst);
        for (size_t i = 0; i < formant.size(); ++i) {
            r.formant[i]   = mix(formant[i], to.formant[i]);
            r.bandwidth[i] = mix(bandwidth[i], to.bandwidth[i]);
        }
        for (size_t i = 0; i < noiseFrequency.size(); ++i) {
            r.noiseFrequency[i] = mix(noiseFrequency[i], to.noiseFrequency[i]);
            r.noiseBandwidth[i] = mix(noiseBandwidth[i], to.noiseBandwidth[i]);
        }
        r.nasalZero = mix(nasalZero, to.nasalZero);
        return r;
    }

    // 音源をすべて止めた状態(声道の形は保持)
    SpeechParams Silenced() const {
        SpeechParams r = *this;
        r.voicing = r.aspiration = r.frication = r.burst = 0.0;
        return r;
    }
};

/// <summary>
/// startからendへ線形に変化する区間
/// </summary>
struct SpeechSegment {
    double duration = 0.0; //< 秒
    SpeechParams start;
    SpeechParams end;
};

}; // KetaEngine
