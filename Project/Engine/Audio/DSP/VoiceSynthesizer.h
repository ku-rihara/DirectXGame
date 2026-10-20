#pragma once

#include "AudioPCM.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace KetaEngine {

/// <summary>
/// フォルマント合成による簡易音声合成(ローマ字入力)
/// </summary>
class VoiceSynthesizer {
public:
    struct Param {
        int sampleRate               = 44100;
        int channelCount             = 2;
        double pitch                 = 200.0;                 //< 声の高さ
        double moraDuration          = 0.3;                   //< 1音の長さ
        double formantBandwidth      = 100.0;                 //< フォルマントの帯域幅
        double vowelFormantScale     = 1.0 / (4.0 * 0.19);    //< 母音フォルマントの倍率
        double consonantFormantScale = 1.0 / (4.0 * 0.17);    //< 子音フォルマントの倍率
        double consonantGain         = 1.0;                   //< 子音フォルマントの強さ
        double deEmphasis            = 0.98;                  //< ディエンファシスの係数
        double fadeTime              = 0.01;                  //< 1音ごとのフェード
        double openRatio             = 0.4;                   //< 声門音源の開大期の割合
        double closeRatio            = 0.16;                  //< 声門音源の閉小期の割合
        bool normalize               = true;                  //< 最後に音量を正規化するか
        double normalizePeak         = 0.8;
    };

    // フォルマン
    using Formant = std::vector<double>;

public:
    VoiceSynthesizer();

    /// <summary>
    /// テキストから音声を合成
    /// </summary>
    /// <param name="text">ローマ字</param>
    AudioPCM Synthesize(const std::string& text) const;

    /// <summary>
    /// 1音分を合成
    /// </summary>
    /// <param name="vowel">母音("a"等)</param>
    /// <param name="consonant">子音("k"等、無ければ空)</param>
    std::vector<double> SynthesizeMora(const std::string& vowel, const std::string& consonant) const;

    // フォルマント表の登録/上書き
    void SetVowelFormant(const std::string& vowel, const Formant& formant) { vowelMap_[vowel] = formant; }
    void SetConsonantFormant(const std::string& consonant, const Formant& formant) { consonantMap_[consonant] = formant; }

    bool IsVowel(char c);
    bool IsConsonant(char c);

private:
    // 声門音源をフォルマントのバンドパスフィルター群に通して足し合わせる
    void ApplyFormant(const std::vector<double>& source, const Formant& formant, double scale, double gain,
        std::vector<double>& output) const;

private:
    Param param_;
    std::unordered_map<std::string, Formant> vowelMap_;
    std::unordered_map<std::string, Formant> consonantMap_;

public:
    Param& GetParam() { return param_; }
    const Param& GetParam() const { return param_; }
    void SetParam(const Param& param) { param_ = param; }
};

}; // KetaEngine
