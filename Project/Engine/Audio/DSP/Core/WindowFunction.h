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

public:
    explicit WindowFunction(Type type = Type::Hanning) : type_(type) {}

    // 窓を生成
    std::vector<double> Generate(int size) const;

    const char* GetTypeName(Type type) const;

private:
    Type type_;

public:
    Type GetType() const { return type_; }
    void SetType(Type type) { type_ = type; }
};

}; // KetaEngine
