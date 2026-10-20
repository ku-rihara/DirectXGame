#pragma once

#include "Audio/DSP/Core/AudioPCM.h"
#include "FormantTable.h"
#include "MoraParser.h"
#include <string>
#include <vector>

namespace KetaEngine {

class ISoundSource;

/// <summary>
/// フォルマント合成による簡易音声合成
/// 音源(ISoundSource) → フォルマント周波数のバンドパスフィルター群 → ディエンファシス
/// </summary>
class VoiceSynthesizer {
public:
    struct Param {
        int sampleRate               = 44100;
        int channelCount             = 2;
        double moraDuration          = 0.3;                //< 1音の長さ[秒]
        double formantBandwidth      = 100.0;              //< フォルマントの帯域幅[Hz]
        double vowelFormantScale     = 1.0 / (4.0 * 0.19); //< 母音フォルマントの倍率(声道長の反映)
        double consonantFormantScale = 1.0 / (4.0 * 0.17); //< 子音フォルマントの倍率
        double consonantGain         = 1.0;                //< 子音フォルマントの強さ
        double deEmphasis            = 0.98;               //< ディエンファシスの係数
        double fadeTime              = 0.01;               //< 1音ごとのフェード[秒]
        bool normalize               = true;               //< 最後に音量を正規化するか
        double normalizePeak         = 0.8;
    };

public:
    /// <summary>
    /// テキストから音声を合成
    /// </summary>
    /// <param name="text">ローマ字</param>
    /// <param name="source">声帯の音源(GlottalSource等)</param>
    AudioPCM Synthesize(const std::string& text, ISoundSource& source) const;

    // 1音分を合成
    std::vector<double> SynthesizeMora(const Mora& mora, ISoundSource& source) const;

private:
    // 音源をフォルマントのバンドパスフィルター群に通して足し合わせる
    void ApplyFormant(const std::vector<double>& source, const FormantTable::Formant& formant, double scale, double gain,
        std::vector<double>& output) const;

private:
    Param param_;
    FormantTable formantTable_;
    MoraParser parser_;

public:
    Param& GetParam() { return param_; }
    const Param& GetParam() const { return param_; }
    void SetParam(const Param& param) { param_ = param; }

    FormantTable& GetFormantTable() { return formantTable_; }
    void SetFormantTable(const FormantTable& table) { formantTable_ = table; }
};

}; // KetaEngine
