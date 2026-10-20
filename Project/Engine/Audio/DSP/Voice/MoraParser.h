#pragma once

#include <string>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 1音(モーラ)。両方空なら無音、母音が空で子音が"n"なら「ん」
/// </summary>
struct Mora {
    std::string vowel;
    std::string consonant;
};

/// <summary>
/// ローマ字テキストをモーラ列に分解する
/// </summary>
class MoraParser {
public:
    /// <summary>
    /// 分解
    /// </summary>
    /// <param name="text">ローマ字(例:"konnichiha")。'-'や' 'は無音1拍、子音の重なりは促音(無音1拍)</param>
    std::vector<Mora> Parse(const std::string& text) const;

private:
    bool IsVowel(char c) const;
    bool IsConsonant(char c) const;

private:
    const std::string vowels_     = "aiueo";
    const std::string consonants_ = "kstnhmyrwgzdbpcfjv";
};

}; // KetaEngine
