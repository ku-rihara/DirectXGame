#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace KetaEngine {

/// <summary>
/// 母音・子音ごとのフォルマント周波数表
/// </summary>
class FormantTable {
public:
    // フォルマント(周波数[Hz]の並び)
    using Formant = std::vector<double>;

public:
    // 既定の表(母音5種、子音k/s/t/n)で初期化
    FormantTable();

    void SetVowel(const std::string& vowel, const Formant& formant) { vowels_[vowel] = formant; }
    void SetConsonant(const std::string& consonant, const Formant& formant) { consonants_[consonant] = formant; }

    // 見つからなければnullptr
    const Formant* FindVowel(const std::string& vowel) const;

    // "ch"等で見つからなければ先頭1文字で再検索、それも無ければnullptr
    const Formant* FindConsonant(const std::string& consonant) const;

private:
    std::unordered_map<std::string, Formant> vowels_;
    std::unordered_map<std::string, Formant> consonants_;
};

}; // KetaEngine
