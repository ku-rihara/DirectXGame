#include "FormantTable.h"

using namespace KetaEngine;

FormantTable::FormantTable() {
    // 母音のフォルマント
    vowels_["a"] = {800, 1200, 2500, 3500};
    vowels_["i"] = {300, 2300, 2900, 3500};
    vowels_["u"] = {300, 1200, 2500, 3500};
    vowels_["e"] = {500, 1900, 2500, 3500};
    vowels_["o"] = {500, 800, 2500, 3500};

    // 子音のフォルマント(仮の値)
    consonants_["k"] = {300, 1500, 2500, 3500};
    consonants_["s"] = {500, 2000, 3000, 4000};
    consonants_["t"] = {600, 2200, 3200, 4200};
    consonants_["n"] = {700, 2400, 3400, 4400};
}

const FormantTable::Formant* FormantTable::FindVowel(const std::string& vowel) const {
    auto it = vowels_.find(vowel);
    return (it != vowels_.end()) ? &it->second : nullptr;
}

const FormantTable::Formant* FormantTable::FindConsonant(const std::string& consonant) const {
    if (consonant.empty()) {
        return nullptr;
    }
    auto it = consonants_.find(consonant);
    if (it == consonants_.end()) {
        it = consonants_.find(consonant.substr(0, 1));
    }
    return (it != consonants_.end()) ? &it->second : nullptr;
}
