#pragma once

#include "AudioPCM.h"

namespace KetaEngine {

/// <summary>
/// 基本波形の生成
/// </summary>
class WaveGenerator {
public:
    enum class Waveform {
        Sine,     //< サイン波
        Sawtooth, //< ノコギリ波
        Square,   //< 矩形波
        Triangle, //< 三角波
        SineSaw,  //< サイン+ノコギリの混合
        Noise,    //< ホワイトノイズ
        Count,
    };

    struct Param {
        Waveform waveform = Waveform::Sine;
        double frequency  = 440.0; //< 周波数[Hz]
        double amplitude  = 0.5;   //< 振幅
        double duration   = 1.0;   //< 長さ[秒]
        double fadeTime   = 0.01;  //< フェードイン/アウト[秒]
    };

    // 波形生成
    static AudioPCM Generate(const Param& param, int sampleRate = 44100, int channelCount = 2);

    /// <summary>
    /// 声門音源(Rosenberg波)の生成
    /// </summary>
    /// <param name="f0">基本周波数(声の高さ)[Hz]</param>
    /// <param name="duration">長さ[秒]</param>
    /// <param name="amplitude">振幅</param>
    /// <param name="openRatio">声門開大期の割合</param>
    /// <param name="closeRatio">声門閉小期の割合</param>
    static std::vector<double> GlottalPulse(int sampleRate, double f0, double duration, double amplitude = 0.2,
        double openRatio = 0.4, double closeRatio = 0.16);

    static const char* GetWaveformName(Waveform waveform);
};

}; // KetaEngine
