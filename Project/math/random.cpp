#include "Random.h"

// 乱数生成器を初期化する関数
std::mt19937& Random::GetEngine() {
    // 静的変数として一度だけ初期化
    static std::random_device rd;
    static std::mt19937 engine(rd());
    return engine;
}

// int型の範囲指定のランダムな値を返す
int Random::Range(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(GetEngine());
}

// float型の範囲指定のランダムな値を返す
float Random::Range(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(GetEngine());
}
