#pragma once
#include <random>

/// <summary>
/// メルセンヌ・ツイスタを用いた乱数生成ユーティリティクラス
/// </summary>
class Random {
public:
    // int型のランダムな値を返す
    static int Range(int min, int max);

    // float型のランダムな値を返す
    static float Range(float min, float max);

private:
    // 乱数生成器（メルセンヌ・ツイスタ）
    static std::mt19937& GetEngine();
};