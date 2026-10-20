#pragma once

#include <cstddef>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 音源(波形を生み出すもの)のインターフェース
/// </summary>
class ISoundSource {
public:
    virtual ~ISoundSource() = default;

    /// <summary>
    /// 波形を生成(呼ぶたびに位相0から生成する)
    /// </summary>
    /// <param name="length">サンプル数</param>
    /// <param name="sampleRate">標本化周波数</param>
    virtual std::vector<double> Generate(size_t length, int sampleRate) = 0;
};

}; // KetaEngine
