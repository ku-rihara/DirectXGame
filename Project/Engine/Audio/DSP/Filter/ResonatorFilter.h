#pragma once

#include "IAudioFilter.h"

namespace KetaEngine {

/// <summary>
/// 2次共振器(Klatt型フォルマント共振器)
/// y(n) = A x(n) + B y(n-1) + C y(n-2)
/// 係数を変えても内部状態を保持するので、フォルマントを時間変化させられる
/// </summary>
class ResonatorFilter : public IAudioFilter {
public:
    /// <summary>
    /// 共振周波数・帯域幅の設定
    /// </summary>
    /// <param name="frequency">共振周波数[Hz]</param>
    /// <param name="bandwidth">帯域幅[Hz]</param>
    /// <param name="sampleRate">標本化周波数</param>
    /// <param name="normalizePeak">true:共振周波数でゲイン1 / false:直流でゲイン1(カスケード接続用)</param>
    void SetParam(double frequency, double bandwidth, int sampleRate, bool normalizePeak = false);

    void Reset() override;
    double ProcessSample(double x) override;

private:
    double a_ = 1.0, b_ = 0.0, c_ = 0.0;
    double y1_ = 0.0, y2_ = 0.0;
};

/// <summary>
/// 2次反共振器(鼻音などのスペクトルの谷を作る)
/// </summary>
class AntiResonatorFilter : public IAudioFilter {
public:
    void SetParam(double frequency, double bandwidth, int sampleRate);

    void Reset() override;
    double ProcessSample(double x) override;

private:
    double a_ = 1.0, b_ = 0.0, c_ = 0.0;
    double x1_ = 0.0, x2_ = 0.0;
};

}; // KetaEngine
