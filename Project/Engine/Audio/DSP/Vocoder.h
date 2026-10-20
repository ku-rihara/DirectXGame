#pragma once

#include "AudioPCM.h"
#include "WindowFunction.h"

namespace KetaEngine {

/// <summary>
/// チャンネルボコーダ
/// モジュレータ(声など)のスペクトル包絡をキャリア(シンセ音など)に掛ける
/// </summary>
class Vocoder {
public:
    struct Param {
        int fftSize                 = 1024; //< DFTサイズ(2のべき乗)
        int bandWidth               = 8;    //< 包絡を平均化するビン数
        WindowFunction::Type window = WindowFunction::Type::Hanning;
        bool normalize              = true;
        double normalizePeak        = 0.8;
    };

public:
    /// <summary>
    /// ボコーダ処理
    /// </summary>
    /// <param name="carrier">キャリア(音程・音色の元)</param>
    /// <param name="modulator">モジュレータ(声など、モノラル化して使用)</param>
    /// <returns>キャリアと同じチャンネル数、短い方の長さ</returns>
    AudioPCM Process(const AudioPCM& carrier, const AudioPCM& modulator) const;

private:
    std::vector<double> ProcessChannel(const std::vector<double>& carrier, const std::vector<double>& modulator) const;

private:
    Param param_;

public:
    Param& GetParam() { return param_; }
    const Param& GetParam() const { return param_; }
    void SetParam(const Param& param) { param_ = param; }
};

}; // KetaEngine
