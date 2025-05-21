#include "Vector2.h"
#include "Vector3.h"
#include "Random.h"
#include<numbers>

template<typename T>
T Shake(const float& t, const float& strength) {
    // t を [0, 1] に正規化
    float normalizedT = std::clamp(t, 0.0f, 1.0f); // t が 0 から 1 の範囲に収まるように
    float adjustedStrength = normalizedT * strength;

    if constexpr (std::is_same<T, float>::value) {
        return Random::Range(-adjustedStrength, adjustedStrength);
    }
    else if constexpr (std::is_same<T, Vector2>::value) {
        return Vector2(
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength)
        );
    }
    else if constexpr (std::is_same<T, Vector3>::value) {
        return Vector3(
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength),
            Random::Range(-adjustedStrength, adjustedStrength)
        );
    }
    else {
        static_assert(false, "Shake cannot cast");
    }
}


template<typename T>
T ShakeWave(const float& t, const float& strength) {
    float normalizedT = std::clamp(t, 0.0f, 1.0f); // 0～1の範囲に収める
    float decay = (1.0f - normalizedT); // 時間とともに減衰
    float frequency = 10.0f; // 振動の周波数

    if constexpr (std::is_same<T, float>::value) {
        float wave = std::sin(frequency * t * std::numbers::pi_v<float>);
        float noise = Random::Range(-1.0f, 1.0f);
        return decay * strength * (wave + 0.2f * noise); // sin波 + ランダム揺れ
    }

    else if constexpr (std::is_same<T, Vector2>::value) {
        float waveX = std::sin(frequency * t * std::numbers::pi_v<float>);
        float waveY = std::cos(frequency * t * std::numbers::pi_v<float>);
        Vector2 noise = {
            Random::Range(-1.0f, 1.0f),
            Random::Range(-1.0f, 1.0f)
        };
        return Vector2(
            decay * strength * (waveX + 0.2f * noise.x),
            decay * strength * (waveY + 0.2f * noise.y)
        );
    }

    else if constexpr (std::is_same<T, Vector3>::value) {
        float waveX = std::sin(frequency * t * std::numbers::pi_v<float>);
        float waveY = std::cos(frequency * t * std::numbers::pi_v<float>);
        float waveZ = std::sin(frequency * t * std::numbers::pi_v<float> /**0.5f*/);
        Vector3 noise = {
            Random::Range(-1.0f, 1.0f),
            Random::Range(-1.0f, 1.0f),
            Random::Range(-1.0f, 1.0f)
        };
        return decay * strength * (Vector3(waveX, waveY, waveZ) + 0.2f * noise);
    }
   
}

template float Shake<float>(const float& t, const float& strength);
template Vector2 Shake<Vector2>(const float& t, const float& strength);
template Vector3 Shake<Vector3>(const float& t, const float& strength);

// 明示的なテンプレートインスタンス化
template float ShakeWave<float>(const float& t, const float& strength);
template Vector2 ShakeWave<Vector2>(const float& t, const float& strength);
template Vector3 ShakeWave<Vector3>(const float& t, const float& strength);
