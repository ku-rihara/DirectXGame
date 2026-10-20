#include "PhonemeTable.h"

using namespace KetaEngine;
#include <algorithm>

PhonemeTable::PhonemeTable() {
    InitVowels();
    InitConsonants();
}

void PhonemeTable::InitVowels() {
    // 日本語母音(成人男性)のフォルマント。F4,F5はほぼ一定
    //                 F1     F2      F3      F4      F5           B1    B2     B3     B4     B5
    vowels_['a'] = {{{750.0, 1250.0, 2650.0, 3500.0, 4500.0}}, {{90.0, 100.0, 140.0, 250.0, 300.0}}};
    vowels_['i'] = {{{280.0, 2250.0, 3000.0, 3650.0, 4500.0}}, {{60.0, 110.0, 160.0, 250.0, 300.0}}};
    vowels_['u'] = {{{330.0, 1350.0, 2450.0, 3450.0, 4500.0}}, {{70.0, 110.0, 150.0, 250.0, 300.0}}};
    vowels_['e'] = {{{460.0, 1950.0, 2650.0, 3550.0, 4500.0}}, {{70.0, 110.0, 160.0, 250.0, 300.0}}};
    vowels_['o'] = {{{480.0, 850.0, 2650.0, 3450.0, 4500.0}}, {{80.0, 100.0, 140.0, 250.0, 300.0}}};
}

void PhonemeTable::InitConsonants() {
    using M = ConsonantSpec::Manner;
    using P = ConsonantSpec::Place;

    // 破裂音: 閉鎖 → 破裂 → (無声なら気息) → 遷移
    auto stop = [](P place, bool voiced, double burstGain, double f1, double bw1, double f2, double bw2) {
        ConsonantSpec s;
        s.manner         = M::Stop;
        s.place          = place;
        s.isVoiced       = voiced;
        s.closureMs      = voiced ? 35.0 : 48.0;
        s.burstMs        = voiced ? 6.0 : 10.0;
        s.noiseMs        = voiced ? 0.0 : 25.0; // 気息
        s.transitionMs   = 38.0;
        s.burstGain      = burstGain * (voiced ? 0.5 : 1.0);
        s.noiseGain      = voiced ? 0.0 : 0.5;
        s.noiseFrequency = {f1, f2};
        s.noiseBandwidth = {bw1, bw2};
        return s;
    };
    consonants_["k"] = stop(P::Velar, false, 0.9, 0.0, 700.0, 0.0, 1500.0); // 周波数は母音から計算
    consonants_["g"] = stop(P::Velar, true, 0.9, 0.0, 700.0, 0.0, 1500.0);
    consonants_["t"] = stop(P::Alveolar, false, 1.0, 4200.0, 1800.0, 6500.0, 2500.0);
    consonants_["d"] = stop(P::Alveolar, true, 1.0, 4200.0, 1800.0, 6500.0, 2500.0);
    consonants_["p"] = stop(P::Labial, false, 0.5, 900.0, 1500.0, 2500.0, 3000.0);
    consonants_["b"] = stop(P::Labial, true, 0.5, 900.0, 1500.0, 2500.0, 3000.0);

    // 摩擦音
    auto fricative = [](P place, bool voiced, double ms, double gain, double f1, double bw1, double f2, double bw2) {
        ConsonantSpec s;
        s.manner         = M::Fricative;
        s.place          = place;
        s.isVoiced       = voiced;
        s.noiseMs        = ms;
        s.transitionMs   = 40.0;
        s.noiseGain      = gain;
        s.noiseFrequency = {f1, f2};
        s.noiseBandwidth = {bw1, bw2};
        return s;
    };
    consonants_["s"]  = fricative(P::Alveolar, false, 95.0, 1.0, 5800.0, 2200.0, 8500.0, 3000.0);
    consonants_["sh"] = fricative(P::Postalveolar, false, 95.0, 1.0, 3000.0, 1400.0, 4800.0, 2500.0);
    consonants_["f"]  = fricative(P::Labial, false, 80.0, 0.35, 1600.0, 3000.0, 5000.0, 4000.0); // ふ(両唇摩擦)

    // 破擦音: 短い閉鎖 → 破裂 → 摩擦
    auto affricate = [](P place, bool voiced, double f1, double bw1, double f2, double bw2) {
        ConsonantSpec s;
        s.manner         = M::Affricate;
        s.place          = place;
        s.isVoiced       = voiced;
        s.closureMs      = voiced ? 28.0 : 38.0;
        s.burstMs        = 5.0;
        s.noiseMs        = voiced ? 40.0 : 55.0;
        s.transitionMs   = 40.0;
        s.burstGain      = 0.6;
        s.noiseGain      = voiced ? 0.6 : 0.9;
        s.noiseFrequency = {f1, f2};
        s.noiseBandwidth = {bw1, bw2};
        return s;
    };
    consonants_["ch"] = affricate(P::Postalveolar, false, 3000.0, 1400.0, 4800.0, 2500.0);
    consonants_["ts"] = affricate(P::Alveolar, false, 5800.0, 2200.0, 8500.0, 3000.0);
    consonants_["j"]  = affricate(P::Postalveolar, true, 3000.0, 1400.0, 4800.0, 2500.0);
    consonants_["z"]  = affricate(P::Alveolar, true, 5800.0, 2200.0, 8500.0, 3000.0);

    // h: 後続母音の形のまま息だけ流す
    {
        ConsonantSpec s;
        s.manner       = M::Aspirate;
        s.place        = P::Glottal;
        s.noiseMs      = 75.0;
        s.transitionMs = 30.0;
        s.noiseGain    = 0.8;
        consonants_["h"] = s;
    }

    // 鼻音
    auto nasal = [](P place, double zero) {
        ConsonantSpec s;
        s.manner       = M::Nasal;
        s.place        = place;
        s.isVoiced     = true;
        s.noiseMs      = 45.0; // 鼻音の持続
        s.transitionMs = 35.0;
        s.nasalZero    = zero;
        return s;
    };
    consonants_["m"] = nasal(P::Labial, 1100.0);
    consonants_["n"] = nasal(P::Alveolar, 1700.0);

    // r: 舌先で一瞬はじく
    {
        ConsonantSpec s;
        s.manner       = M::Flap;
        s.place        = P::Alveolar;
        s.isVoiced     = true;
        s.noiseMs      = 18.0;
        s.transitionMs = 35.0;
        consonants_["r"] = s;
    }

    // 半母音
    auto glide = [](P place) {
        ConsonantSpec s;
        s.manner       = M::Glide;
        s.place        = place;
        s.isVoiced     = true;
        s.noiseMs      = 30.0;
        s.transitionMs = 70.0;
        return s;
    };
    consonants_["y"] = glide(P::Palatal);
    consonants_["w"] = glide(P::Labial);
}

const VowelTarget* PhonemeTable::FindVowel(char vowel) const {
    auto it = vowels_.find(vowel);
    return (it != vowels_.end()) ? &it->second : nullptr;
}

const ConsonantSpec* PhonemeTable::FindConsonant(const std::string& consonant) const {
    auto it = consonants_.find(consonant);
    return (it != consonants_.end()) ? &it->second : nullptr;
}

PhonemeTable::Locus PhonemeTable::GetLocus(ConsonantSpec::Place place, const VowelTarget& vowel) const {
    using P = ConsonantSpec::Place;
    switch (place) {
    case P::Labial:
        return {250.0, 800.0, 2200.0};
    case P::Alveolar:
        return {250.0, 1750.0, 2700.0};
    case P::Postalveolar:
        return {250.0, 2100.0, 2900.0};
    case P::Palatal:
        return {270.0, 2250.0, 3000.0};
    case P::Velar: {
        // 軟口蓋音はF2とF3が近づく(後続母音のF2に依存)
        const double f2 = std::clamp(vowel.formant[1] + 300.0, 1400.0, 2400.0);
        return {250.0, f2, f2 + 350.0};
    }
    case P::Glottal:
    default:
        return {vowel.formant[0], vowel.formant[1], vowel.formant[2]};
    }
}
