#include "MoraParser.h"

using namespace KetaEngine;
#include <cctype>

std::vector<Mora> MoraParser::Parse(const std::string& text) const {
    std::vector<Mora> moras;
    std::string consonant;

    for (char raw : text) {
        const char c = static_cast<char>(std::tolower(static_cast<unsigned char>(raw)));

        if (c == '-' || c == ' ') {
            // 無音1拍
            moras.push_back({});
            consonant.clear();
        } else if (IsVowel(c)) {
            moras.push_back({std::string(1, c), consonant});
            consonant.clear();
        } else if (IsConsonant(c)) {
            if (consonant == "n" && c != 'y') {
                // 「ん」
                moras.push_back({"", "n"});
                consonant = std::string(1, c);
            } else if (!consonant.empty() && consonant.back() == c) {
                // 「っ」(促音)は無音1拍
                moras.push_back({});
                consonant = std::string(1, c);
            } else {
                consonant += c;
            }
        }
    }
    if (consonant == "n") {
        moras.push_back({"", "n"});
    }
    return moras;
}

bool MoraParser::IsVowel(char c) const {
    return vowels_.find(c) != std::string::npos;
}

bool MoraParser::IsConsonant(char c) const {
    return consonants_.find(c) != std::string::npos;
}
