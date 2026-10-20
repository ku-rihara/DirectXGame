#pragma once

#include "ISoundSource.h"

namespace KetaEngine {

/// <summary>
/// 声門音源(Rosenberg波)
/// </summary>
class GlottalSource : public ISoundSource {
public:
    struct Param {
        double pitch      = 200.0; //< 基本周波数(声の高さ)[Hz]
        double amplitude  = 0.2;   //< 振幅
        double openRatio  = 0.4;   //< 声門開大期の割合(1周期に対する)
        double closeRatio = 0.16;  //< 声門閉小期の割合(1周期に対する)
    };

public:
    GlottalSource() = default;
    explicit GlottalSource(const Param& param) : param_(param) {}

    std::vector<double> Generate(size_t length, int sampleRate) override;

private:
    Param param_;

public:
    Param& GetParam() { return param_; }
    const Param& GetParam() const { return param_; }
    void SetParam(const Param& param) { param_ = param; }
};

}; // KetaEngine
