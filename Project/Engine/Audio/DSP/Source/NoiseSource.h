#pragma once

#include "ISoundSource.h"
#include <random>

namespace KetaEngine {

/// <summary>
/// ホワイトノイズ音源(-amplitude～amplitude)
/// </summary>
class NoiseSource : public ISoundSource {
public:
    explicit NoiseSource(double amplitude = 1.0);

    std::vector<double> Generate(size_t length, int sampleRate) override;

    // 1サンプル生成
    double NextSample();

private:
    double amplitude_;
    std::mt19937 random_;
    std::uniform_real_distribution<double> dist_{-1.0, 1.0};

public:
    void SetAmplitude(double amplitude) { amplitude_ = amplitude; }
};

}; // KetaEngine
