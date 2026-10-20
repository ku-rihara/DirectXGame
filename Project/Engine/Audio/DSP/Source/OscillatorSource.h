#pragma once

#include "ISoundSource.h"
#include <random>

namespace KetaEngine {

/// <summary>
/// 基本波形の発振器
/// </summary>
class OscillatorSource : public ISoundSource {
public:
    enum class Waveform {
        Sine,     //< サイン波
        Sawtooth, //< ノコギリ波
        Square,   //< 矩形波
        Triangle, //< 三角波
        SineSaw,  //< サイン+ノコギリの混合(ボコーダのキャリア向け)
        Noise,    //< ホワイトノイズ
        Count,
    };

    struct Param {
        Waveform waveform = Waveform::Sine;
        double frequency  = 440.0; //< 周波数[Hz]
        double amplitude  = 0.5;   //< 振幅
    };

public:
    OscillatorSource();
    explicit OscillatorSource(const Param& param);

    std::vector<double> Generate(size_t length, int sampleRate) override;

    const char* GetWaveformName(Waveform waveform) const;

private:
    // 位相(0～1)から1サンプル分の値を求める
    double Sample(double phase);

private:
    Param param_;
    std::mt19937 random_;
    std::uniform_real_distribution<double> noiseDist_{-1.0, 1.0};

public:
    Param& GetParam() { return param_; }
    const Param& GetParam() const { return param_; }
    void SetParam(const Param& param) { param_ = param; }
};

}; // KetaEngine
