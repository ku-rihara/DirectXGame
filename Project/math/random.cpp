#include "Random.h"
#include <algorithm> // std::swap

// int型の範囲指定のランダムな値を返す
int Random::Range(int min, int max) {
    // min と max を正しい順序にする
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(GetEngine());
}

// float型の範囲指定のランダムな値を返す
float Random::Range(float min, float max) {
    // min と max を正しい順序にする
    if (min > max) std::swap(min, max);
    std::uniform_real_distribution<float> dist(min, max);
    return dist(GetEngine());
}

// 乱数エンジンを取得
std::mt19937& Random::GetEngine() {
    static std::mt19937 engine{ std::random_device{}() };
    return engine;
}
