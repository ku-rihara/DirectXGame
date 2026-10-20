#pragma once

#include <array>
#include <string>
#include <unordered_map>

namespace KetaEngine {

/// <summary>
/// 母音の目標値
/// </summary>
struct VowelTarget {
    std::array<double, 5> formant;   //< F1～F5[Hz]
    std::array<double, 5> bandwidth; //< 帯域幅[Hz]
};

/// <summary>
/// 子音の発音方法と各区間の長さ
/// </summary>
struct ConsonantSpec {
    // 調音方法
    enum class Manner {
        Stop,      //< 破裂音 k t p g d b
        Fricative, //< 摩擦音 s sh f z(語中)
        Affricate, //< 破擦音 ch ts j z
        Aspirate,  //< 声門摩擦 h
        Nasal,     //< 鼻音 m n
        Flap,      //< はじき音 r
        Glide,     //< 半母音 y w
    };
    // 調音位置(フォルマント遷移の起点 = ローカスを決める)
    enum class Place {
        Labial,       //< 唇
        Alveolar,     //< 歯茎
        Postalveolar, //< 後部歯茎
        Palatal,      //< 硬口蓋
        Velar,        //< 軟口蓋
        Glottal,      //< 声門
    };

    Manner manner = Manner::Stop;
    Place place   = Place::Alveolar;
    bool isVoiced = false;

    // 各区間の長さ[ms]
    double closureMs    = 0.0;  //< 閉鎖(無音/声帯振動のみ)
    double burstMs      = 0.0;  //< 破裂
    double noiseMs      = 0.0;  //< 摩擦/気息/鼻音/半母音の保持
    double transitionMs = 40.0; //< 母音へのフォルマント遷移

    // 強さ
    double burstGain = 0.0;
    double noiseGain = 0.0;

    // 摩擦/破裂ノイズの共振(0なら母音から自動計算)
    std::array<double, 2> noiseFrequency = {0.0, 0.0};
    std::array<double, 2> noiseBandwidth = {2000.0, 2000.0};

    double nasalZero = 270.0; //< 鼻音の反共振周波数[Hz]
};

/// <summary>
/// 日本語の母音・子音の音響パラメータ表
/// </summary>
class PhonemeTable {
public:
    using Locus = std::array<double, 3>; //< F1～F3

public:
    // 既定値(成人男性の日本語)で初期化
    PhonemeTable();

    const VowelTarget* FindVowel(char vowel) const;
    const ConsonantSpec* FindConsonant(const std::string& consonant) const;

    void SetVowel(char vowel, const VowelTarget& target) { vowels_[vowel] = target; }
    void SetConsonant(const std::string& consonant, const ConsonantSpec& spec) { consonants_[consonant] = spec; }

    // 調音位置ごとのローカス(子音側のフォルマントの起点)
    Locus GetLocus(ConsonantSpec::Place place, const VowelTarget& vowel) const;

private:
    void InitVowels();
    void InitConsonants();

private:
    std::unordered_map<char, VowelTarget> vowels_;
    std::unordered_map<std::string, ConsonantSpec> consonants_;
};

}; // KetaEngine
