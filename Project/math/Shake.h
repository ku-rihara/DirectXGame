#include "Random.h"
#include "Vector2.h"
#include "Vector3.h"
#include <numbers>

template <typename T>
T Shake(float t, float strength) {
    // t を [0, 1] に正規化
    float normalizedT      = std::clamp(t, 0.0f, 1.0f); // t が 0 から 1 の範囲に収まるように
    float adjustedStrength = normalizedT * strength;

    if constexpr (std::is_same<T, float>::value) {
        return Random::Range(-adjustedStrength, adjustedStrength);
    } else if constexpr (std::is_same<T, Vector2>::value) {
        return Vector2(
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength));
    } else if constexpr (std::is_same<T, Vector3>::value) {
        return Vector3(
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength));
    } else {
        static_assert(false, "Shake cannot cast");
    }
}

template <typename T>
T ShakeWave(float t, float strength) {
    float normalizedT = std::clamp(t, 0.0f, 1.0f);
    float growth      = normalizedT; // 小さい時は弱く、時間と共に強くなる
    float frequency   = 10.0f;

    if constexpr (std::is_same<T, float>::value) {
        float wave  = std::sin(frequency * t * std::numbers::pi_v<float>);
        float noise = Random::Range(-1.0f, 1.0f);
        return growth * strength * (wave + 0.2f * noise);
    } else if constexpr (std::is_same<T, Vector2>::value) {
        float waveX   = std::sin(frequency * t * std::numbers::pi_v<float>);
        float waveY   = std::cos(frequency * t * std::numbers::pi_v<float>);
        Vector2 noise = {
            Random::Range(-1.0f, 1.0f),
            Random::Range(-1.0f, 1.0f)};
        return Vector2(
            growth * strength * (waveX + 0.2f * noise.x),
            growth * strength * (waveY + 0.2f * noise.y));
    } else if constexpr (std::is_same<T, Vector3>::value) {
        float waveX   = std::sin(frequency * t * std::numbers::pi_v<float>);
        float waveY   = std::cos(frequency * t * std::numbers::pi_v<float>);
        float waveZ   = std::sin(frequency * t * std::numbers::pi_v<float> * 0.5f);
        Vector3 noise = {
            Random::Range(-1.0f, 1.0f),
            Random::Range(-1.0f, 1.0f),
            Random::Range(-1.0f, 1.0f)};
        return growth * strength * (Vector3(waveX, waveY, waveZ) + 0.2f * noise);
    }
}

template float Shake<float>(float t, float strength);
template Vector2 Shake<Vector2>(float t, float strength);
template Vector3 Shake<Vector3>(float t, float strength);

// 明示的なテンプレートインスタンス化
template float ShakeWave<float>(float t, float strength);
template Vector2 ShakeWave<Vector2>(float t, float strength);
template Vector3 ShakeWave<Vector3>(float t, float strength);
