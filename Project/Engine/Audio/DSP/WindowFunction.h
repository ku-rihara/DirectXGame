#pragma once

#include <vector>

namespace KetaEngine {

/// <summary>
/// 窓関数
/// </summary>
class WindowFunction {
public:
    enum class Type {
        Rectangular, //< 矩形窓
        Hanning,     //< ハニング窓
        Hamming,     //< ハミング窓
        Blackman,    //< ブラックマン窓
        Count,
    };

    /// <summary>
    /// 窓関数を生成
    /// </summary>
    /// <param name="type">窓の種類</param>
    /// <param name="size">サイズ</param>
    static std::vector<double> Generate(Type type, int size);

    static const char* GetTypeName(Type type);
};

}; // KetaEngine
