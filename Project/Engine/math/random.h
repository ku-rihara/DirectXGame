#pragma once
#include <random>

class Random {
public:
    // int型のランダムな値を返す（minからmaxまで）
    static int Range(int min, int max);

    // float型のランダムな値を返す（minからmaxまで）
    static float Range(float min, float max);

private:
    // 乱数生成器（メルセンヌ・ツイスタ）
    static std::mt19937& GetEngine();
};